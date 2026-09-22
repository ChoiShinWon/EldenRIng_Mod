#include "EldenRing_Mod/Component/EldenHitboxComponent.h"
#include "EldenRing_Mod/Component/EldenPoiseComponent.h"
#include "EldenRing_Mod/Character/EldenEnemy.h"
#include "EldenRing_Mod/Weapon/EldenShield.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "Kismet/GameplayStatics.h"


UEldenHitboxComponent::UEldenHitboxComponent()
{
	

	// 초기 콜리전 세팅
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetCollisionResponseToAllChannels(ECR_Ignore);
	
	SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	
}

void UEldenHitboxComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UEldenHitboxComponent::OnHitboxOverlap);
}

void UEldenHitboxComponent::OnHitboxOverlap(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;

	// 내 주인이 방패라면 데미지를 주지 않고 함수 종료
	if (Cast<AEldenShield>(GetOwner())) return;

	// 1. 방패의 물리 히트박스에 닿는 건 쿨하게 무시.
	if (Cast<AEldenShield>(OtherActor)) return;

	AActor* HitboxOwner = GetOwner();
	// 히트박스는 보통 무기 액터에 붙어있고, 그 무기의 진짜 주인은
	// 무기를 들고 있는 캐릭터(GetOwner())임. 무기 자체가 소유주가 없는 특수 케이스를 대비해
	// 없으면 히트박스 자신을 공격자로 취급
	AActor* ActualAttacker = HitboxOwner->GetOwner() ? HitboxOwner->GetOwner() : HitboxOwner;

	// 자기 자신이나 같은 클래스(적) 때리는 것 방지
	if (OtherActor == HitboxOwner || OtherActor == ActualAttacker) return;

	// 주의: 클래스가 정확히 일치할 때만 아군으로 판정.
	// 적 종류가 늘어나서 AEldenEnemy를 상속하는 여러 서브클래스가 생기면
	// 서로 다른 서브클래스끼리는 이 체크를 통과 못해서 아군 오사가 발생할 수 있음.
	// 지금은 적이 전부 같은 클래스라서 우연히 맞물려 동작하는 상태
	if (Cast<AEldenEnemy>(ActualAttacker) && Cast<AEldenEnemy>(OtherActor)) return; 

	// 다단히트 방지: 한 번의 스윙 (EnableHitbox->DisableHitbox 구간) 동안
	// 이미 맞은 대상은 HitActors 배열에 쌓아두고 재타격을 막음.
	// 이 배열은 EnableHitbox()가 호출될 때만 비워지므로, 스윙 1회 = Enable/Disable 1쌍이라는
	// 전제가 깨지면 이전 스윙의 다단히트 방지 상태가 새지 않음.
	if (HitActors.Contains(OtherActor)) return;
	HitActors.Add(OtherActor);


	// 이펙트 스폰 위치 계산
	FVector SpawnLocation = GetComponentLocation(); // 기본값: 내 위치

	if (bFromSweep)
	{
		SpawnLocation = SweepResult.ImpactPoint;
	}
	else if (OtherComp != nullptr)
	{
		FVector ClosestPoint;
		OtherComp->GetClosestPointOnCollision(SpawnLocation, ClosestPoint);
		SpawnLocation = ClosestPoint;
	}

	//  2. 즉시 데미지 전달 
	UGameplayStatics::ApplyDamage(OtherActor, DamageAmount, GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());

	if (AEldenEnemy* HitEnemy = Cast<AEldenEnemy>(OtherActor))
	{
		if (HitEnemy->PoiseComp && !HitEnemy->bIsStunned && !HitEnemy->GetIsDead())
		{
			HitEnemy->PoiseComp->ApplyPoiseDamage(PoiseDamage);
		}
	}

	//  3. ApplyDamage -> TargetPlayer::TakeDamage가 이미 동기적으로 실행된 뒤이므로,
	// 여기서는 방금 그 데미지 처리 중에 방어 이벤트가 발생했는지를 플래그로 물어봄
	// 패리 성공 > 회피 무적 판정 > 가드 성공 순으로 검사
	// 세 상태가 동시에 true일 수 있는 프레임에서
	// 가장 강한 방어 판정이 우선해서 이펙트/사운드가 중복 재생되지 않게 하기 위함
	// 패리/회피는 자체 이펙트를 별도로 재생하므로 여기선 조용히 return
	// 가드만 여기서 직접 Impact 이펙트(깡 소리)를 재생.
	if (AEldenCharacter* TargetPlayer = Cast<AEldenCharacter>(OtherActor))
	{
		if (TargetPlayer->bParrySucceeded)
		{
			// 패리 성공후 false로 즉시 전환
			TargetPlayer->bParrySucceeded = false;
			return;
		}

		if (TargetPlayer->bDodgeInvincibleHit)
		{
			// 구르기 무적 이후 즉시 false로 전환
			TargetPlayer->bDodgeInvincibleHit = false;
			return;
		}

		if (TargetPlayer->bShieldBlockedAttack)
		{
			// 가드 성공후 false로 즉시 전환
			TargetPlayer->bShieldBlockedAttack = false; // 신호 초기화

			// 방어 성공 이펙트 및 깡! 소리 재생
			PlayImpactEffects(GuardVFX, GuardSound, SpawnLocation);

			return; //  피 튀기는 로직으로 넘어가지 않고 여기서 깔끔하게 종료
		}
	}

	//  4. 위 방어 판정에 전부 걸리지 않았다면 일반 피격
	PlayImpactEffects(HitVFX, HitSound, SpawnLocation);
}

void UEldenHitboxComponent::PlayImpactEffects(UParticleSystem* VFX, USoundBase* Sound, const FVector& Location) const
{
	if (VFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), VFX, Location);
	}
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location);
	}
}

void UEldenHitboxComponent::EnableHitbox()
{
	HitActors.Empty();
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECR_Overlap);
}


void UEldenHitboxComponent::DisableHitbox()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

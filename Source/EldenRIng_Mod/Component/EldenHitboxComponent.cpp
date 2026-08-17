// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/Component/EldenHitboxComponent.h"
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

	// 1. 방패의 물리 히트박스에 닿는 건 쿨하게 무시합니다. (플레이어 몸통을 때리게 냅둠)
	if (Cast<AEldenShield>(OtherActor)) return;

	AActor* HitboxOwner = GetOwner();
	AActor* ActualAttacker = HitboxOwner->GetOwner() ? HitboxOwner->GetOwner() : HitboxOwner;

	// 자기 자신이나 같은 클래스(적) 때리는 것 방지
	if (OtherActor == HitboxOwner || OtherActor == ActualAttacker) return;
	if (ActualAttacker && ActualAttacker->GetClass() == OtherActor->GetClass()) return;

	// 다단히트 방지
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

	//  3. TakeDamage 실행 결과, 대상(플레이어)이 가드에 성공했는지 물어봄
	if (AEldenCharacter* TargetPlayer = Cast<AEldenCharacter>(OtherActor))
	{
		if (TargetPlayer->bShieldBlockedAttack)
		{
			TargetPlayer->bShieldBlockedAttack = false; // 신호 초기화

			// 방어 성공 이펙트 및 깡! 소리 재생
			PlayImpactEffects(GuardVFX, GuardSound, SpawnLocation);

			return; //  피 튀기는 로직으로 넘어가지 않고 여기서 깔끔하게 종료
		}
	}

	//  4. 가드를 안 했거나, 뒤통수 맞았거나, 가드 붕괴(스태미나 0)라면 일반 피격 이펙트 재생!
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
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

	if (Cast<AEldenShield>(OtherActor))
	{
		return;
	}
	AActor* HitboxOwner = GetOwner();

	FVector SpawnLocation = GetComponentLocation();
	if (bFromSweep) SpawnLocation = SweepResult.ImpactPoint;
	else if (OtherComp != nullptr)
	{
		FVector ClosestPoint;
		OtherComp->GetClosestPointOnCollision(SpawnLocation, ClosestPoint);
		SpawnLocation = ClosestPoint;
	}

	if (AEldenShield* ShieldOwner = Cast<AEldenShield>(HitboxOwner))
	{
		AEldenCharacter* PlayerCharacter = Cast<AEldenCharacter>(ShieldOwner->GetOwner());
		if (OtherActor == PlayerCharacter || OtherActor == ShieldOwner) return;
		if (PlayerCharacter && OtherActor->GetOwner() == PlayerCharacter) return;

		UEldenHitboxComponent* EnemyWeaponHitbox = Cast<UEldenHitboxComponent>(OtherComp);
		if (!EnemyWeaponHitbox) return;

		if (PlayerCharacter)
		{
			EnemyWeaponHitbox->HitActors.Add(PlayerCharacter);

			// 방패가 실제로 칼을 막아냈으므로 플레이어에게 "방패 방어 성공" 신호를 보낸다
			PlayerCharacter->bShieldBlockedAttack = true;
		}

	    PlayImpactEffects(GuardVFX, GuardSound, SpawnLocation);
		

		return;
	}

	AActor* ActualAttacker = HitboxOwner->GetOwner() ? HitboxOwner->GetOwner() : HitboxOwner;
	

	if (OtherActor == HitboxOwner || OtherActor == ActualAttacker) return;
	if (ActualAttacker && ActualAttacker->GetClass() == OtherActor->GetClass()) return;
	if (HitActors.Contains(OtherActor)) return;

	if (AEldenCharacter* TargetPlayer = Cast<AEldenCharacter>(OtherActor))
	{
		if (TargetPlayer->bShieldBlockedAttack)
		{
			TargetPlayer->bShieldBlockedAttack = false; // 신호 초기화
			return; // 피격 소리, 이펙트, 데미지 모두 발생시키지 않고 여기서 깔끔하게 차단!
		}
	}

	HitActors.Add(OtherActor);

	UGameplayStatics::ApplyDamage(OtherActor,DamageAmount,GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
	

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
// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/Component/EldenHitboxComponent.h"
#include <Kismet/GameplayStatics.h>


UEldenHitboxComponent::UEldenHitboxComponent()
{
	

	// 초기 콜리전 세팅
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
}

void UEldenHitboxComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UEldenHitboxComponent::OnHitboxOverlap);
}

void UEldenHitboxComponent::OnHitboxOverlap(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor == nullptr || OtherActor == GetOwner())
	{
		return;
	}

	if (HitActors.Contains(OtherActor))
	{
		return;
	}

	HitActors.Add(OtherActor);
	UGameplayStatics::ApplyDamage(OtherActor,DamageAmount,GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
	
	if (HitVFX)
	{
		FVector SpawnLocation = GetComponentLocation();
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
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitVFX, SpawnLocation);
	}
	
	

}

void UEldenHitboxComponent::EnableHitbox()
{
	HitActors.Empty();
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UEldenHitboxComponent::DisableHitbox()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
#include "EldenRing_Mod/Component/LockOnComponent.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

ULockOnComponent::ULockOnComponent() { PrimaryComponentTick.bCanEverTick = false; }

void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<AEldenCharacter>(GetOwner());
}

void ULockOnComponent::ToggleLockOn()
{
	// 여기서 주인(캐릭터)을 바로 찾습니다.
	if (!OwnerCharacter) return;

	// 락온중일때
	if (CurrentTarget.IsValid())
	{
		
		IITargetable* TargetInterface = GetTargetInterface();
		// 락온 마크가 있다면 마크 보여주기
		if (TargetInterface) TargetInterface->ShowTargetMark(false);
		// 락온 대상 비우기
		CurrentTarget = nullptr;
		OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
	// 락온중이지 않을때
	else
	{
		FindBestTarget();
		if (CurrentTarget.IsValid())
		{

			IITargetable* TargetInterface = GetTargetInterface();
			if (TargetInterface) TargetInterface->ShowTargetMark(true);
			OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
			OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		}
	}
}

void ULockOnComponent::UpdateLockOn(float DeltaTime)
{
	if (!OwnerCharacter || !CurrentTarget.IsValid()) return;
	IITargetable* TargetInterface = GetTargetInterface();
	if (!TargetInterface || !TargetInterface->IsTargetable())
	{
		ToggleLockOn(); // 해제
		return;
	}

	float Distance = FVector::DistSquared(OwnerCharacter->GetActorLocation(), CurrentTarget->GetActorLocation());
	if (Distance > MaxLockOnDistance * MaxLockOnDistance)
	{
		ToggleLockOn(); // 해제
		return;
	}
	AController* Controller = OwnerCharacter->GetController();
	if (!Controller) return;

	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(OwnerCharacter->GetActorLocation(), CurrentTarget->GetActorLocation());
	LookAtRot.Pitch -= 15.0f;
	FRotator SmoothRot = FMath::RInterpTo(Controller->GetControlRotation(), LookAtRot, DeltaTime, 5.0f);

	Controller->SetControlRotation(SmoothRot);

	
}


void ULockOnComponent::FindBestTarget()
{
	if (!OwnerCharacter) return;

	TArray<AActor*> FoundTargets;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UITargetable::StaticClass(), FoundTargets);
	AActor* Closest = nullptr;
	float ClosestDist = LockOnAcquireDistance;

	for (AActor* Actor : FoundTargets)
	{
		IITargetable* Target = Cast<IITargetable>(Actor);
		if (Target && Target->IsTargetable())
		{
			float Dist = FVector::Dist(OwnerCharacter->GetActorLocation(), Actor->GetActorLocation());
			if (Dist < ClosestDist)
			{
				ClosestDist = Dist;
				Closest = Actor;
			}
		}
	}
	CurrentTarget = Closest;
}

IITargetable* ULockOnComponent::GetTargetInterface() const
{
	if (!CurrentTarget.Get()) return nullptr;
	IITargetable* TargetInterface = Cast<IITargetable>(CurrentTarget.Get());

	return TargetInterface;
}

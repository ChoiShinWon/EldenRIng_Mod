#include "EldenRing_Mod/Component/LockOnComponent.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Character/EldenEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

ULockOnComponent::ULockOnComponent() { PrimaryComponentTick.bCanEverTick = true; }

void ULockOnComponent::ToggleLockOn()
{
	// 여기서 주인(캐릭터)을 바로 찾습니다.
	AEldenCharacter* OwnerChar = Cast<AEldenCharacter>(GetOwner());
	if (!OwnerChar) return;

	if (CurrentTarget)
	{
		IITargetable* TargetInterface = Cast<IITargetable>(CurrentTarget);
		if (TargetInterface) TargetInterface->ShowTargetMark(false);
		CurrentTarget = nullptr;
		OwnerChar->GetCharacterMovement()->bOrientRotationToMovement = true;
		OwnerChar->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
	else
	{
		FindBestTarget();
		if (CurrentTarget)
		{
			IITargetable* TargetInterface = Cast<IITargetable>(CurrentTarget);
			if (TargetInterface) TargetInterface->ShowTargetMark(true);
			OwnerChar->GetCharacterMovement()->bOrientRotationToMovement = false;
			OwnerChar->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		}
	}
}

void ULockOnComponent::UpdateLockOn(float DeltaTime)
{
	AEldenCharacter* OwnerChar = Cast<AEldenCharacter>(GetOwner());
	if (!OwnerChar || !CurrentTarget) return;

	AEldenEnemy* Enemy = Cast<AEldenEnemy>(CurrentTarget);
	if (!IsValid(CurrentTarget) || (Enemy && Enemy->GetIsDead()))
	{
		ToggleLockOn(); // 해제
		return;
	}

	float Distance = FVector::DistSquared(OwnerChar->GetActorLocation(), CurrentTarget->GetActorLocation());
	if (Distance > MaxLockOnDistance * MaxLockOnDistance)
	{
		ToggleLockOn(); // 해제
		return;
	}

	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(OwnerChar->GetActorLocation(), CurrentTarget->GetActorLocation());
	LookAtRot.Pitch -= 15.0f;
	FRotator SmoothRot = FMath::RInterpTo(OwnerChar->GetController()->GetControlRotation(), LookAtRot, DeltaTime, 5.0f);
	OwnerChar->GetController()->SetControlRotation(SmoothRot);

	
}

void ULockOnComponent::FindBestTarget()
{
	AEldenCharacter* OwnerChar = Cast<AEldenCharacter>(GetOwner());
	if (!OwnerChar) return;

	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEldenEnemy::StaticClass(), FoundEnemies);

	AActor* Closest = nullptr;
	float MinDist = 1500.0f;

	for (AActor* Actor : FoundEnemies)
	{
		AEldenEnemy* Enemy = Cast<AEldenEnemy>(Actor);
		if (Enemy && !Enemy->GetIsDead())
		{
			float Dist = FVector::Dist(OwnerChar->GetActorLocation(), Actor->GetActorLocation());
			if (Dist < MinDist) { MinDist = Dist; Closest = Actor; }
		}
	}
	CurrentTarget = Closest;
}
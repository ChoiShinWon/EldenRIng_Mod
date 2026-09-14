#include "EldenRing_Mod/Component/LockOnComponent.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Character/EldenEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

ULockOnComponent::ULockOnComponent() { PrimaryComponentTick.bCanEverTick = true; }

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
		IITargetable* TargetInterface = Cast<IITargetable>(CurrentTarget.Get());
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
			IITargetable* TargetInterface = Cast<IITargetable>(CurrentTarget.Get());
			if (TargetInterface) TargetInterface->ShowTargetMark(true);
			OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
			OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		}
	}
}

void ULockOnComponent::UpdateLockOn(float DeltaTime)
{
	if (!OwnerCharacter || !CurrentTarget.IsValid()) return;

	if (CurrentTarget->GetIsDead())
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

	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEldenEnemy::StaticClass(), FoundEnemies);

	AEldenEnemy* Closest = nullptr;
	float MinDist = 1500.0f;

	for (AActor* Actor : FoundEnemies)
	{
		AEldenEnemy* Enemy = Cast<AEldenEnemy>(Actor);
		if (Enemy && !Enemy->GetIsDead())
		{
			float Dist = FVector::Dist(OwnerCharacter->GetActorLocation(), Actor->GetActorLocation());
			if (Dist < MinDist) { MinDist = Dist; Closest = Enemy; }
		}
	}
	CurrentTarget = Closest;
}

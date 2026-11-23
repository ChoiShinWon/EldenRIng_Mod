#include "EldenRing_Mod/Component/LockOnComponent.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Subsystem/EldenTargetSubsystem.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

ULockOnComponent::ULockOnComponent() { PrimaryComponentTick.bCanEverTick = false; }

void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<AEldenCharacter>(GetOwner());
	if (OwnerCharacter) LockOnMark = OwnerCharacter->LockOnMark;
}

void ULockOnComponent::ToggleLockOn()
{
	if (!OwnerCharacter) return;
	if (CurrentTarget.IsValid())
	{
		ReleaseLockOn();
		return;
	}
	else
	{
		FindBestTarget();
		if (CurrentTarget.IsValid())
		{
			IITargetable* TargetInterface = GetTargetInterface();
			if (TargetInterface) TargetInterface->ShowTargetMark(true);
			if (IsValid(LockOnMark))
			{
				LockOnMark->SetVisibility(true);
				LockOnMark->SetWorldLocation(CurrentTarget->GetActorLocation());
			}
			OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
			OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
			OwnerCharacter->SetActorTickEnabled(true);
		}
	}
}

void ULockOnComponent::UpdateLockOn(float DeltaTime)
{
	if (!OwnerCharacter) return;
	if (!CurrentTarget.IsValid())
	{
		ReleaseLockOn();
		return;
	}

	IITargetable* TargetInterface = GetTargetInterface();
	if (!TargetInterface || !TargetInterface->IsTargetable())
	{
		ReleaseLockOn(); // 해제
		return;
	}

	float Distance = FVector::DistSquared(OwnerCharacter->GetActorLocation(), CurrentTarget->GetActorLocation());
	if (Distance > MaxLockOnDistance * MaxLockOnDistance)
	{
		ReleaseLockOn(); // 해제
		return;
	}
	AController* Controller = OwnerCharacter->GetController();
	if (!Controller) return;

	if (IsValid(LockOnMark)) LockOnMark->SetWorldLocation(CurrentTarget->GetActorLocation());
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(OwnerCharacter->GetActorLocation(), CurrentTarget->GetActorLocation());
	LookAtRot.Pitch -= 15.0f;
	FRotator SmoothRot = FMath::RInterpTo(Controller->GetControlRotation(), LookAtRot, DeltaTime, 5.0f);

	Controller->SetControlRotation(SmoothRot);

}

// 락온 후보 중 가장 가까운 적을 찾아 CurrentTarget에 넣는다.
// 락온 키를 누를 때만 호출
void ULockOnComponent::FindBestTarget()
{
	if (!OwnerCharacter) return;
	if (!GetWorld()) return;
	
	UEldenTargetSubsystem* TargetSub = GetWorld()->GetSubsystem<UEldenTargetSubsystem>();
	if (!TargetSub) return;
	

	// 기준값
	// 거리 비교는 전부 "제곱 거리"로 한다.
	// 그래서 사거리도 제곱해서 맞춘다
	const FVector PlayerLocation = OwnerCharacter->GetActorLocation();
	double ClosestDistSq = FMath::Square(LockOnAcquireDistance);
	AActor* Closest = nullptr;

	// 후보 순회
	for (const TWeakObjectPtr<AActor>& Weak : TargetSub->GetTargets())
	{
		// 목록은 약참조라 이미 파괴된 적이 남아 있을 수 있다. 반드시 null 확인
		AActor* Actor = Weak.Get();
		if (!Actor) continue;

		double DistSq = FVector::DistSquared(PlayerLocation, Actor->GetActorLocation());

		// 싼 검사(거리) 먼저: 현재 1등보다 멀거나 사거리 밖이면 탈락.
		// 비싼 인터페이스 캐스트는 후보가 된 적에게만 한다.
		if (DistSq >= ClosestDistSq) continue;

		// 락온 가능한 상태인지 확인 (죽은 적 등은 IsTargetable() 이 걸러줌)
		IITargetable* Target = Cast<IITargetable>(Actor);
		if (!Target || !Target->IsTargetable()) continue;

		// 여기까지 왔으면 "지금까지의1등보다 가깝고 락온 가능한 적"
		ClosestDistSq = DistSq;
		Closest = Actor;
	}

	CurrentTarget = Closest;

}

IITargetable* ULockOnComponent::GetTargetInterface() const
{
	if (!CurrentTarget.Get()) return nullptr;
	IITargetable* TargetInterface = Cast<IITargetable>(CurrentTarget.Get());

	return TargetInterface;
}

void ULockOnComponent::ReleaseLockOn()
{
	if (!OwnerCharacter) return;
	IITargetable* Target = GetTargetInterface();
	if (Target) Target->ShowTargetMark(false);
	CurrentTarget = nullptr;

	// 표식 숨기기
	if (IsValid(LockOnMark))
	{
		LockOnMark->SetVisibility(false);
	}

	// 이동 모드 복구
	OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
	OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;

	// 플레이어 Tick 끄기
	OwnerCharacter->SetActorTickEnabled(false);

}

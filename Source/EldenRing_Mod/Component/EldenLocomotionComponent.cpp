#include "EldenRing_Mod/Component/EldenLocomotionComponent.h"
#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Component/EldenCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"

UEldenLocomotionComponent::UEldenLocomotionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

}


void UEldenLocomotionComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<AEldenCharacter>(GetOwner());
	if (!OwnerCharacter) return;
	CachedStatComp = OwnerCharacter->StatComponent;
	CMC = OwnerCharacter->GetCharacterMovement();

}


void UEldenLocomotionComponent::UpdateTickState()
{
	SetComponentTickEnabled(bIsSprinting || bIsLunging);
}

void UEldenLocomotionComponent::Dodge()
{
	if (!OwnerCharacter) return;

	// 스태미너 부족 or 구르는중이면 무시
	if (CachedStatComp->GetCurrentStamina() < DodgeStaminaCost || OwnerCharacter->GetState() == ECharacterState::Rolling) return;

	// 공격 중이면 예약만 하고 끝
	if (OwnerCharacter->GetState() == ECharacterState::Attacking)
	{
		OwnerCharacter->CombatComponent->QueueDodge();
		return;
	}

	// 몽타주 준비
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance || !RollMontage) return;

	// 스태미너 소모 + SetState(Rolling) + 회전3종 해제
	CachedStatComp->ConsumeStamina(DodgeStaminaCost);
	OwnerCharacter->SetState(ECharacterState::Rolling);

	CMC->bOrientRotationToMovement = false;
	CMC->bUseControllerDesiredRotation = false;
	OwnerCharacter->bUseControllerRotationYaw = false;

	AController* Controller = OwnerCharacter->GetController();
	FVector DodgeDir = OwnerCharacter->GetVelocity().GetSafeNormal();

	//만약 제자리에 서서 구르기만 눌렀다면?
	if (DodgeDir.IsNearlyZero() && !LastMoveInput.IsNearlyZero())
	{
		if (Controller != nullptr)
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			DodgeDir = (ForwardDirection * LastMoveInput.Y + RightDirection * LastMoveInput.X).GetSafeNormal();
		}
	}
	else if (DodgeDir.IsNearlyZero() && LastMoveInput.IsNearlyZero())
	{
		DodgeDir = OwnerCharacter->GetActorForwardVector();
	}

	// 구를 방향으로 회전값 계산
	FRotator DodgeRotation = DodgeDir.Rotation();
	DodgeRotation.Pitch = 0.0f; // 바닥으로 처박히는 것 방지
	DodgeRotation.Roll = 0.0f;

	// 캐릭터 몸통을 즉시 강제로 돌려버림!
	OwnerCharacter->SetActorRotation(DodgeRotation, ETeleportType::TeleportPhysics);

	// 몽타주 재생
	AnimInstance->Montage_Play(RollMontage);

	// --- 구르기 종료 감지 예약 ---
	FOnMontageEnded RollEndDelegate;
	RollEndDelegate.BindUObject(this, &UEldenLocomotionComponent::OnRollMontageEnded);
	AnimInstance->Montage_SetEndDelegate(RollEndDelegate, RollMontage);


}

void UEldenLocomotionComponent::OnRollMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!OwnerCharacter) return;
	OwnerCharacter->SetState(ECharacterState::Idle);
	if (OwnerCharacter->GetIsLockedOn())
	{
		CMC->bUseControllerDesiredRotation = true;
		CMC->bOrientRotationToMovement = false;
	}
	else
	{
		// 평소 모드 복구
		CMC->bUseControllerDesiredRotation = false;
		CMC->bOrientRotationToMovement = true;
	}
}

void UEldenLocomotionComponent::StartSprint()
{
	if (!OwnerCharacter) return;
	if (OwnerCharacter->GetState() == ECharacterState::Drinking) return;

	if (CachedStatComp->GetCurrentStamina() > 0.0f)
	{
		bIsSprinting = true;
		CMC->MaxWalkSpeed = 800.0f;
	}
	UpdateTickState();
}

void UEldenLocomotionComponent::StopSprint()
{
	if (!OwnerCharacter || !bIsSprinting) return;

	bIsSprinting = false;
	CMC->MaxWalkSpeed = 500.0f;
	UpdateTickState();
}

void UEldenLocomotionComponent::StartAttackLunge(float Speed)
{
	if (!OwnerCharacter) return;
	bIsLunging = true;

	// 돌진이 끝난 뒤, 원래 걷기 속도로 복원해야 하므로 속도를 덮어쓰기 전에 기존 값을 저장해야 함
	SavedWalkSpeedBeforeLunge = CMC->MaxWalkSpeed;
	// 저장한 후에 스피드 값 갱신
	CMC->MaxWalkSpeed = Speed;
	UpdateTickState();
}

void UEldenLocomotionComponent::StopAttackLunge()
{
	if (!OwnerCharacter) return;

	bIsLunging = false;

	// 몽타주 재생 시 돌진이 끝난 뒤 원래 걷기 속도로 복구
	CMC->MaxWalkSpeed = SavedWalkSpeedBeforeLunge;
	UpdateTickState();
}

void UEldenLocomotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!OwnerCharacter) return;

	if (bIsSprinting && OwnerCharacter->GetVelocity().Size() > 0.0f)
	{
		CachedStatComp->ConsumeStamina(SprintStaminaCost * DeltaTime);

		// 달리다가 스태미너 떨어지면 멈춤
		if (CachedStatComp->GetCurrentStamina() <= 0.0f)
		{
			StopSprint();
		}
	}

	if (bIsLunging)
	{
		// 애니메이션 에셋 Root Motion 오류로, 이동은 코드가 매 프레임 밀어준다.
		// AddMovementInput은 "방향 + 세기"만 제출
		// 실제 속도는 CharacterMovementComponent의 MaxWalkSpeed, MaxAcceleration이 결정
		OwnerCharacter->AddMovementInput(OwnerCharacter->GetActorForwardVector(), 1.0f);
	}

}

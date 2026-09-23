

#include "EldenRing_Mod/Component/EldenGraceRestComponent.h"
#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UEldenGraceRestComponent::UEldenGraceRestComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UEldenGraceRestComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AEldenCharacter>(GetOwner());

	
}

void UEldenGraceRestComponent::EnterRest()
{
	if (!OwnerCharacter) return;
	OwnerCharacter->SetHUDVisible(false);
	OwnerCharacter->SetEquippedItemsHidden(true);

	OwnerCharacter->GetCharacterMovement()->StopMovementImmediately();
	OwnerCharacter->SetState(ECharacterState::Interacting);

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

	if (!AnimInstance) return;
	AnimInstance->Montage_Play(SitMontage);
}

void UEldenGraceRestComponent::ExitRest()
{
	if (!OwnerCharacter) return;
	// 단순 게터는 헤더로 안빼도 됨
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

	if (!AnimInstance) return;
	AnimInstance->Montage_Play(StandUpMontage);

	// 델리게이트 등록
	FOnMontageEnded StandUpEndDelegate;
	StandUpEndDelegate.BindUObject(this, &UEldenGraceRestComponent::OnStandUpMontageEnded);
	AnimInstance->Montage_SetEndDelegate(StandUpEndDelegate, StandUpMontage);

	if (APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController()))
	{
		PC->SetViewTargetWithBlend(OwnerCharacter, ExitCameraBlendTime);
	}
	OwnerCharacter->InventoryComponent->RefillPotions();
	OwnerCharacter->StatComponent->FullRestore();
	OwnerCharacter->SetHUDVisible(true);
	OwnerCharacter->SetEquippedItemsHidden(false);

}

void UEldenGraceRestComponent::OnStandUpMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!OwnerCharacter) return;
	OwnerCharacter->SetState(ECharacterState::Idle);
}



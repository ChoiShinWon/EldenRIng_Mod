#include "EldenRing_Mod/Component/EldenItemUseComponent.h"
#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "EldenRing_Mod/Component/EldenCombatComponent.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "Animation/AnimInstance.h"

UEldenItemUseComponent::UEldenItemUseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UEldenItemUseComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AEldenCharacter>(GetOwner());
	if (!OwnerCharacter) return;
	CachedInventoryComp = OwnerCharacter->InventoryComponent;
	CachedStatComp = OwnerCharacter->StatComponent;
	
}

void UEldenItemUseComponent::UseItem()
{
	if (!OwnerCharacter) return;
	// 1. 공통 예외 처리 (어떤 아이템이든 구르거나 죽어있을 땐 못 씀)
	if (OwnerCharacter->GetState() != ECharacterState::Idle) return;

	// 2. 인벤토리에게 현재 장착된 아이템이 뭔지 물어봄
	EConsumableType CurrentItem = CachedInventoryComp->GetCurrentSelectedItem();

	// 3. 아이템 종류에 따라 다른 행동(로직) 실행
	switch (CurrentItem)
	{
	case EConsumableType::HP_Potion:
	{
		//
		if (CachedStatComp->IsHealthFull()) return;
		StartDrinkingPotion();

		break;
	}

	case EConsumableType::Mana_Potion:
	{
		if (CachedStatComp->IsManaFull()) return;
		StartDrinkingPotion();
		break;
	}

	case EConsumableType::None:
	default:
		// 아이템이 없을 때는 아무것도 안 함 (혹은 빈 슬롯을 만지는 애니메이션 재생)
		UE_LOG(LogTemp, Warning, TEXT("빈 슬롯입니다!"));
		break;
	}
}

void UEldenItemUseComponent::SwitchItem()
{
	if (!OwnerCharacter) return;

	if (OwnerCharacter->GetState() == ECharacterState::Drinking) return;
	CachedInventoryComp->SelectNextItem();
}

void UEldenItemUseComponent::StartDrinkingPotion()
{
	if (!OwnerCharacter) return;

	if (!CachedInventoryComp->CanUseItem()) return;


	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	UAnimMontage* UseMontage = CachedInventoryComp->GetCurrentUseMontage();
	if (!AnimInstance || !UseMontage) return;

	AnimInstance->Montage_Play(UseMontage);
	FOnMontageEnded PotionEndDelegate;
	PotionEndDelegate.BindUObject(this, &UEldenItemUseComponent::OnPotionMontageEnded);
	AnimInstance->Montage_SetEndDelegate(PotionEndDelegate, UseMontage);

	OwnerCharacter->SetState(ECharacterState::Drinking);

	if (OwnerCharacter->CombatComponent->bIsSprinting) OwnerCharacter->StopSprint();
	OwnerCharacter->SetDrinkingVisuals(true);
}

void UEldenItemUseComponent::ApplyItemEffect()
{
	if (!OwnerCharacter) return;

	// 노티파이 실행 시점에도 현재 아이템이 뭔지 확인하고 해당 효과를 적용
	EConsumableType CurrentItem = CachedInventoryComp->GetCurrentSelectedItem();

	switch (CurrentItem)
	{
	case EConsumableType::HP_Potion:
	{
		CachedInventoryComp->ConsumeItem();
		float RestoreAmount = CachedInventoryComp->GetPotionRestoreAmount();
		CachedStatComp->Heal(RestoreAmount);
		break;
	}
	case EConsumableType::Mana_Potion:
	{
		CachedInventoryComp->ConsumeItem();
		float RestoreAmount = CachedInventoryComp->GetPotionRestoreAmount();
		CachedStatComp->RestoreMana(RestoreAmount);
		break;
	}
	}
}

void UEldenItemUseComponent::OnPotionMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!OwnerCharacter) return;
	OwnerCharacter->SetDrinkingVisuals(false);
	if (OwnerCharacter->GetState() == ECharacterState::Drinking)
	{
		OwnerCharacter->SetState(ECharacterState::Idle);
	}
}

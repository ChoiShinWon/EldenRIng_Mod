#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "Engine/Texture.h"

UEldenInventoryComponent::UEldenInventoryComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;

	
}


void UEldenInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if(!ItemSlots.IsValidIndex(SelectedIndex)) SelectedIndex = 0;
	RefillPotions();
}

bool UEldenInventoryComponent::CanUseItem() const
{
	const FEldenItemSlot* Slot = GetSelectedSlot();
	return Slot && Slot->Count > 0;
}

void UEldenInventoryComponent::ConsumeItem()
{
	if (!CanUseItem()) return;
	GetSelectedSlot()->Count--;
	BroadcastPotionCount();
}

float UEldenInventoryComponent::GetPotionHealAmount() const
{
	const UEldenItemDefinition* Def = GetSelectedDefinition();
	return Def ? Def->HealAmount : 0.0f;
}

void UEldenInventoryComponent::RefillPotions()
{
	for (FEldenItemSlot& Slot : ItemSlots)
	{
		Slot.Count = Slot.Definition ? Slot.Definition->MaxCount : 0;
	}
	
	BroadcastPotionCount();
}

UTexture2D* UEldenInventoryComponent::GetCurrentItemIcon() const
{
	const UEldenItemDefinition* Def = GetSelectedDefinition();
	return Def ? Def->Icon : nullptr;
}

void UEldenInventoryComponent::SetSelectedIndex(int32 NewIndex)
{
	SelectedIndex = NewIndex;
	OnSelectedItemChanged.Broadcast();
	BroadcastPotionCount();
}

void UEldenInventoryComponent::SelectNextItem()
{
	// 아이템 슬롯이 0개나 1개면 바꿀 게 없음 -> 그냥 return
	if (ItemSlots.Num() <= 1) return;

	SetSelectedIndex((SelectedIndex + 1) % ItemSlots.Num());
}


void UEldenInventoryComponent::BroadcastPotionCount()
{
	OnPotionCountChanged.Broadcast(GetCurrentPotionCount(), GetMaxPotionCount());
}

const FEldenItemSlot* UEldenInventoryComponent::GetSelectedSlot() const
{
	return ItemSlots.IsValidIndex(SelectedIndex) ? &ItemSlots[SelectedIndex] : nullptr;
}

FEldenItemSlot* UEldenInventoryComponent::GetSelectedSlot()
{
	return ItemSlots.IsValidIndex(SelectedIndex) ? &ItemSlots[SelectedIndex] : nullptr;
}

const UEldenItemDefinition* UEldenInventoryComponent::GetSelectedDefinition() const
{
	const FEldenItemSlot* Slot = GetSelectedSlot();
	return Slot ? Slot->Definition : nullptr; 
}

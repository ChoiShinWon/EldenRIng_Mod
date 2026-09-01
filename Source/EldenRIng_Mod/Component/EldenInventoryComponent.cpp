#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "Engine/Texture.h"

UEldenInventoryComponent::UEldenInventoryComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;

	
}


void UEldenInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentPotionCount = EquippedItem ? EquippedItem->MaxCount : 0;
	BroadcastPotionCount();
}

bool UEldenInventoryComponent::CanUseItem() const
{
	return CurrentPotionCount > 0;
}

void UEldenInventoryComponent::ConsumeItem()
{
	
	if (CurrentPotionCount > 0)
	{
		CurrentPotionCount--;
		BroadcastPotionCount();
	}
}

float UEldenInventoryComponent::GetPotionHealAmount() const
{
	return EquippedItem ? EquippedItem->HealAmount : 0.0f;
}

void UEldenInventoryComponent::RefillPotions()
{
	CurrentPotionCount = EquippedItem ? EquippedItem->MaxCount : 0;
	BroadcastPotionCount();
}

UTexture2D* UEldenInventoryComponent::GetCurrentItemIcon() const
{
	return EquippedItem ? EquippedItem->Icon : nullptr;
}

void UEldenInventoryComponent::BroadcastPotionCount()
{
	OnPotionCountChanged.Broadcast(CurrentPotionCount, GetMaxPotionCount());
}

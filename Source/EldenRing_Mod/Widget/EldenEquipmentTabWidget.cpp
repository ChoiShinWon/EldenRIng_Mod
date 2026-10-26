#include "EldenRing_Mod/Widget/EldenEquipmentTabWidget.h"
#include "EldenRing_Mod/Widget/EldenItemSlotWidget.h"
#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "EldenRing_Mod/Component/EldenEquipmentComponent.h"
#include "EldenRing_Mod/Item/EldenItemDefinition.h"
#include "EldenRing_Mod/Weapon/EldenWeapon.h"
#include "EldenRing_Mod/Weapon/EldenShield.h"
#include "Components/UniformGridPanel.h"

void UEldenEquipmentTabWidget::RefreshEquipment(UEldenInventoryComponent* Inventory, class UEldenEquipmentComponent* Equipment)
{
	CachedInventoryComponent = Inventory;
	if (!Inventory) return;

	TArray<TObjectPtr<UEldenItemDefinition>> RightHandItems;
	if (Equipment)
	{
		for (AEldenWeapon* Weapon : Equipment->GetSpawnedWeapons())
		{
			if (Weapon)
			{
				RightHandItems.Add(Weapon->GetItemDefinition());
			}
		}
	}

	TArray<TObjectPtr<UEldenItemDefinition>> LeftHandItems;
	if (Equipment && Equipment->GetEquippedShield())
	{
		for (AEldenShield* Shield : Equipment->GetSpawnedShields())
		{
			if (Shield)
			{
				LeftHandItems.Add(Shield->GetItemDefinition());
			}
		}
	}


	RefreshCategorySlots(RightHandGridPanel, RightHandSlotCount, RightHandSlotCount, &RightHandItems, false);
	RefreshCategorySlots(LeftHandGridPanel, LeftHandSlotCount, LeftHandSlotCount, &LeftHandItems, false);
	RefreshCategorySlots(TalismanGridPanel, TalismanSlotCount, TalismanSlotCount, nullptr, false);
	RefreshCategorySlots(ConsumableGridPanel, ConsumableSlotCount, ConsumableColumnCount, &Inventory->GetEquippedConsumables(), true);

}

void UEldenEquipmentTabWidget::OnEquipmentSlotClicked(UEldenItemDefinition* ClickedItem, int32 SlotIndex)
{
	OnSelectedItemClicked.Broadcast(ClickedItem, SlotIndex);
}

void UEldenEquipmentTabWidget::RefreshCategorySlots(UUniformGridPanel* Panel, int32 SlotCount, int32 ColumnCount, const TArray<TObjectPtr<UEldenItemDefinition>>* EquippedData, bool bShowCount)
{
	if (!Panel || !EquipmentSlotWidgetClass) return;
	Panel->ClearChildren();

	for (int32 Index = 0; Index < SlotCount; Index++)
	{

		UEldenItemDefinition* CurrentItem = (EquippedData && Index < EquippedData->Num()) ? (*EquippedData)[Index] : nullptr;
			
		UEldenItemSlotWidget* NewSlot = CreateWidget<UEldenItemSlotWidget>(this, EquipmentSlotWidgetClass);
		if (!NewSlot) continue;

		int32 CurrentCount = CurrentItem ? CachedInventoryComponent->GetItemCount(CurrentItem) : 0;


	
		NewSlot->SetItemData(CurrentItem, CurrentCount, Index, bShowCount);
		NewSlot->OnSelectedItemClicked.AddDynamic(this,
			&UEldenEquipmentTabWidget::OnEquipmentSlotClicked);

		int32 Row = Index / ColumnCount;
		int32 Col = Index % ColumnCount;
		Panel->AddChildToUniformGrid(NewSlot, Row, Col);
		
	}
}

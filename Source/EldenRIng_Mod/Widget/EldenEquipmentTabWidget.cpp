#include "EldenRing_Mod/Widget/EldenEquipmentTabWidget.h"
#include "EldenRing_Mod/Widget/EldenItemSlotWidget.h"
#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "EldenRing_Mod/Item/EldenItemDefinition.h"
#include "Components/UniformGridPanel.h"

void UEldenEquipmentTabWidget::RefreshEquipment(UEldenInventoryComponent* Inventory)
{
	CachedInventoryComponent = Inventory;
	if (!Inventory || !EquipmentGridPanel || !EquipmentSlotWidgetClass) return;
	EquipmentGridPanel->ClearChildren();

	const TArray<TObjectPtr<UEldenItemDefinition>>& Equipped = Inventory->GetEquippedConsumables();
	int32 SlotCount = Inventory->GetMaxEquippedSlots();

	for (int32 Index = 0; Index < SlotCount; Index++)
	{
		UEldenItemDefinition* CurrentItem = (Index < Equipped.Num()) ? Equipped[Index] : nullptr;
		UEldenItemSlotWidget* NewSlot = CreateWidget<UEldenItemSlotWidget>(this, EquipmentSlotWidgetClass);

		if (NewSlot)
		{
			int32 CurrentCount = CurrentItem ? Inventory->GetItemCount(CurrentItem) : 0;

			NewSlot->SetItemData(CurrentItem, CurrentCount, Index);
			NewSlot->OnSelectedItemClicked.AddDynamic(this,
				&UEldenEquipmentTabWidget::OnEquipmentSlotClicked);

			int32 Row = Index / ColumnCount;
			int32 Col = Index % ColumnCount;
			EquipmentGridPanel->AddChildToUniformGrid(NewSlot, Row, Col);
		}
	}
}

void UEldenEquipmentTabWidget::OnEquipmentSlotClicked(UEldenItemDefinition* ClickedItem, int32 SlotIndex)
{
	OnSelectedItemClicked.Broadcast(ClickedItem, SlotIndex);
}

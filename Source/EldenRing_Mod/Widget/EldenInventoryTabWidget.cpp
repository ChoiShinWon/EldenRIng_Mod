

#include "EldenRing_Mod/Widget/EldenInventoryTabWidget.h"
#include "EldenRing_Mod/Widget/EldenItemSlotWidget.h"
#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "EldenRing_Mod/Item/EldenItemDefinition.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void UEldenInventoryTabWidget::RefreshInventory(UEldenInventoryComponent* Inventory)
{
	CachedInventoryComponent = Inventory;


	if (!Inventory || !ItemGridPanel || !ItemSlotWidgetClass) return;
	ItemGridPanel->ClearChildren();

	const TArray<TObjectPtr<UEldenItemDefinition>>& Order = Inventory->GetItemOrder();
	for (int32 Index = 0 ; Index < Order.Num(); Index++)
	{
		UEldenItemDefinition* CurrentItem = Order[Index];
		UEldenItemSlotWidget* NewSlot = CreateWidget<UEldenItemSlotWidget>(this, ItemSlotWidgetClass);
		if (NewSlot)
		{
			NewSlot->OnSelectedItemClicked.AddDynamic(this, &UEldenInventoryTabWidget::OnInventorySlotClicked);
			NewSlot->SetItemData(CurrentItem, Inventory->GetItemCount(CurrentItem), Index);
			int32 Row = Index / ColumnCount;
			int32 Col = Index % ColumnCount;
			ItemGridPanel->AddChildToUniformGrid(NewSlot, Row, Col);
		}
	}
}

void UEldenInventoryTabWidget::OnInventorySlotClicked(UEldenItemDefinition* ClickedItem, int32 SlotIndex)
{
	OnSelectedItemClicked.Broadcast(ClickedItem, SlotIndex);
}

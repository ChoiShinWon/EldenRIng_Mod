#include "EldenRing_Mod/Widget/EldenMenuWidget.h"
#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "EldenRing_Mod/Widget/EldenInventoryTabWidget.h"
#include "EldenRing_Mod/Widget/EldenEquipmentTabWidget.h"
#include "EldenRing_Mod/Item/EldenItemDefinition.h"


void UEldenMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EquipmentTab->OnSelectedItemClicked.AddDynamic(this, &UEldenMenuWidget::OnEquipmentSlotClicked);
	InventoryTab->OnSelectedItemClicked.AddDynamic(this, &UEldenMenuWidget::OnInventoryItemClicked);

	
}

void UEldenMenuWidget::OnEquipmentSlotClicked(UEldenItemDefinition* ClickedItem, int32 SlotIndex)
{
	WaitingEquipmentIndex = SlotIndex;
}

void UEldenMenuWidget::OnInventoryItemClicked(UEldenItemDefinition* ClickedItem, int32 SlotIndex)
{
	if (WaitingEquipmentIndex != -1)
	{
		CachedInventoryComponent->EquipConsumable(ClickedItem);
		WaitingEquipmentIndex = -1;
		InventoryTab->RefreshInventory(CachedInventoryComponent);
		EquipmentTab->RefreshEquipment(CachedInventoryComponent);

	}
}

void UEldenMenuWidget::InitMenu(UEldenInventoryComponent* Inventory)
{
	CachedInventoryComponent = Inventory;
	InventoryTab->RefreshInventory(Inventory);
	EquipmentTab->RefreshEquipment(Inventory);
}

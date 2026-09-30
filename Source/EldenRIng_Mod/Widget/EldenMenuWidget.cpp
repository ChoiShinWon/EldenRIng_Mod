#include "EldenRing_Mod/Widget/EldenMenuWidget.h"
#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "EldenRing_Mod/Component/EldenEquipmentComponent.h"
#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "EldenRing_Mod/Widget/EldenInventoryTabWidget.h"
#include "EldenRing_Mod/Widget/EldenEquipmentTabWidget.h"
#include "EldenRing_Mod/Item/EldenItemDefinition.h"
#include "EldenRing_Mod/Weapon/EldenShield.h"
#include "EldenRing_Mod/Weapon/EldenWeapon.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Button.h"


void UEldenMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EquipmentTab->OnSelectedItemClicked.AddDynamic(this, &UEldenMenuWidget::OnEquipmentSlotClicked);
	InventoryTab->OnSelectedItemClicked.AddDynamic(this, &UEldenMenuWidget::OnInventoryItemClicked);

	PopupCatcherButton->OnClicked.AddDynamic(this, &UEldenMenuWidget::CloseItemActionPopup);
	
}

void UEldenMenuWidget::OnEquipmentSlotClicked(UEldenItemDefinition* ClickedItem, int32 SlotIndex)
{
	OpenItemActionPopup(ClickedItem, EItemActionType::UnEquip);
}

void UEldenMenuWidget::OnInventoryItemClicked(UEldenItemDefinition* ClickedItem, int32 SlotIndex)
{
	if (!ClickedItem) return;
	if (ClickedItem->ItemType == EItemType::GoldenRune)
	{
		OpenItemActionPopup(ClickedItem, EItemActionType::Use);
		return;
	}
	OpenItemActionPopup(ClickedItem, EItemActionType::Equip);
}

void UEldenMenuWidget::InitMenu(UEldenInventoryComponent* Inventory, UEldenStatComponent* Stat, UEldenEquipmentComponent* Equipment)
{
	CachedInventoryComponent = Inventory;
	CachedStatComponent = Stat;
	CachedEquipmentComponent = Equipment;
	InventoryTab->RefreshInventory(Inventory);
	EquipmentTab->RefreshEquipment(Inventory, Equipment);
}

void UEldenMenuWidget::OnItemActionConfirmed(UEldenItemDefinition* Item, EItemActionType ActionType)
{
	if (ActionType == EItemActionType::Equip)
	{
		if (Item->WeaponClass)
		{
			CachedEquipmentComponent->EquipWeapon(Item);
		}
		else if (Item->ShieldClass)
		{
			CachedEquipmentComponent->EquipShield(Item);
		}
		else
		{
			CachedInventoryComponent->EquipConsumable(Item);

		}
	}
	else if (ActionType == EItemActionType::UnEquip)
	{

		if (Item->WeaponClass)
		{
			CachedEquipmentComponent->UnequipWeaponByItem(Item);
		}
		else if (Item->ShieldClass)
		{
			CachedEquipmentComponent->UnequipShieldByItem(Item);
		}
		else
		{
			CachedInventoryComponent->UnequipConsumable(Item);

		}
	}
	else if (ActionType == EItemActionType::Use)
	{
		CachedStatComponent->AddRunes(Item->RuneAmount);
		CachedInventoryComponent->RemoveItem(Item, 1);
	}
	InventoryTab->RefreshInventory(CachedInventoryComponent);
	EquipmentTab->RefreshEquipment(CachedInventoryComponent, CachedEquipmentComponent);
	CloseItemActionPopup();
}

void UEldenMenuWidget::OpenItemActionPopup(UEldenItemDefinition* Item, EItemActionType ActionType)
{
	if (!Item) return;
	if (ActiveItemPopup) CloseItemActionPopup();
	if (!ItemActionPopupClass) return;
	ActiveItemPopup = CreateWidget<UEldenItemActionPopupWidget>(this, ItemActionPopupClass);

	ActiveItemPopup->SetPopup(Item, ActionType);

	ActiveItemPopup->OnActionConfirmed.BindDynamic(this, &UEldenMenuWidget::OnItemActionConfirmed);

	FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());

	UCanvasPanelSlot* PopupCanvasSlot = PopupLayer->AddChildToCanvas(ActiveItemPopup);
	PopupCanvasSlot->SetAnchors(FAnchors(0.f, 0.f));
	PopupCanvasSlot->SetPosition(MousePos + FVector2D(10, 10));
	PopupCanvasSlot->SetAutoSize(true);

	PopupCatcherButton->SetVisibility(ESlateVisibility::Visible);
}

void UEldenMenuWidget::CloseItemActionPopup()
{
	if (ActiveItemPopup)
	{
		ActiveItemPopup->RemoveFromParent();
		ActiveItemPopup = nullptr;
		PopupCatcherButton->SetVisibility(ESlateVisibility::Collapsed);
	}

}


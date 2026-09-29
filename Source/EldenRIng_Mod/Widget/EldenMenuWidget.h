#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EldenRing_Mod/Widget/EldenItemActionPopupWidget.h"
#include "EldenMenuWidget.generated.h"

class UEldenInventoryTabWidget;
class UEldenEquipmentTabWidget;
class UEldenInventoryComponent;
class UEldenItemDefinition;
class UEldenStatComponent;

UCLASS()
class ELDENRING_MOD_API UEldenMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UEldenInventoryTabWidget* InventoryTab;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UEldenEquipmentTabWidget* EquipmentTab;

	UPROPERTY(VisibleAnywhere)
	class UEldenInventoryComponent* CachedInventoryComponent;

	UPROPERTY(VisibleAnywhere)
	class UEldenStatComponent* CachedStatComponent;


	UFUNCTION()
	void OnEquipmentSlotClicked(UEldenItemDefinition* ClickedItem, int32 SlotIndex);

	UFUNCTION()
	void OnInventoryItemClicked(UEldenItemDefinition* ClickedItem, int32 SlotIndex);

	UPROPERTY(EditDefaultsOnly, Category = "Popup")
	TSubclassOf<class UEldenItemActionPopupWidget> ItemActionPopupClass;

	UPROPERTY()
	class UEldenItemActionPopupWidget* ActiveItemPopup;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* PopupLayer;


	UPROPERTY(meta = (BindWidget))
	class UButton* PopupCatcherButton;
public:
	void InitMenu(UEldenInventoryComponent* Inventory, UEldenStatComponent* Stat);

	void OpenItemActionPopup(UEldenItemDefinition* Item, EItemActionType ActionType);

	UFUNCTION()
	void CloseItemActionPopup();

	UFUNCTION()
	void OnItemActionConfirmed(UEldenItemDefinition* Item, EItemActionType ActionType);
};

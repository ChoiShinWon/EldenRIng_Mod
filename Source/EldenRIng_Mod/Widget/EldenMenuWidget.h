#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EldenMenuWidget.generated.h"

class UEldenInventoryTabWidget;
class UEldenEquipmentTabWidget;
class UEldenInventoryComponent;
class UEldenItemDefinition;

UCLASS()
class ELDENRING_MOD_API UEldenMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UEldenInventoryTabWidget* InventoryTab;

	UPROPERTY(meta = (BindWidget))
	class UEldenEquipmentTabWidget* EquipmentTab;

	UPROPERTY(VisibleAnywhere)
	class UEldenInventoryComponent* CachedInventoryComponent;

	UPROPERTY()
	int32 WaitingEquipmentIndex = -1;

	UFUNCTION()
	void OnEquipmentSlotClicked(UEldenItemDefinition* ClickedItem, int32 SlotIndex);

	UFUNCTION()
	void OnInventoryItemClicked(UEldenItemDefinition* ClickedItem, int32 SlotIndex);

public:
	void InitMenu(UEldenInventoryComponent* Inventory);
};

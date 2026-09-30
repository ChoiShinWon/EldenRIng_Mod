#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EldenRing_Mod/Widget/EldenItemSlotWidget.h"
#include "EldenEquipmentTabWidget.generated.h"

class UUniformGridPanel;
class UEldenInventoryComponent;
class UEldenEquipmentComponent;
class UEldenItemSlotWidget;
class UEldenItemDefinition;


UCLASS()
class ELDENRING_MOD_API UEldenEquipmentTabWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* RightHandGridPanel;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* LeftHandGridPanel;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* TalismanGridPanel;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* ConsumableGridPanel;

	// 슬롯 위젯 하나하나를 어떤 블루프린트 클래스로 만들지 결정
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<UEldenItemSlotWidget> EquipmentSlotWidgetClass;


	UPROPERTY(EditDefaultsOnly, Category = "Item")
	int32 RightHandSlotCount = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	int32 LeftHandSlotCount = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	int32 TalismanSlotCount = 4;

	// 소모품만 "칸 수"와 "한 줄 폭"이 다르므로 둘 다 필요
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	int32 ConsumableSlotCount = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	int32 ConsumableColumnCount = 5;

public:
	UPROPERTY()
	class UEldenInventoryComponent* CachedInventoryComponent;

	void RefreshEquipment(UEldenInventoryComponent* Inventory, class UEldenEquipmentComponent* Equipment);

	UPROPERTY()
	FOnItemClickedDelegate OnSelectedItemClicked;

	UFUNCTION()
	void OnEquipmentSlotClicked(UEldenItemDefinition* ClickedItem, int32 SlotIndex);

private:
	void RefreshCategorySlots(UUniformGridPanel* Panel, int32 SlotCount, int32 ColumnCount,
		const TArray<TObjectPtr<UEldenItemDefinition>>* EquippedData);
};

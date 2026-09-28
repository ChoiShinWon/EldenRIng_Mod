#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EldenRing_Mod/Widget/EldenItemSlotWidget.h"
#include "EldenEquipmentTabWidget.generated.h"

class UUniformGridPanel;
class UEldenInventoryComponent;
class UEldenItemSlotWidget;
class UEldenItemDefinition;
UCLASS()
class ELDENRING_MOD_API UEldenEquipmentTabWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// 슬롯 위젯들을 담을 그리드 패널
	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* EquipmentGridPanel;

	// 슬롯 위젯 하나하나를 어떤 블루프린트 클래스로 만들지 결정
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<UEldenItemSlotWidget> EquipmentSlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	int32 ColumnCount = 5;

public:
	UPROPERTY()
	class UEldenInventoryComponent* CachedInventoryComponent;

	void RefreshEquipment(UEldenInventoryComponent* Inventory);

	UPROPERTY()
	FOnItemClickedDelegate OnSelectedItemClicked;

	UFUNCTION()
	void OnEquipmentSlotClicked(UEldenItemDefinition* ClickedItem, int32 SlotIndex);


};

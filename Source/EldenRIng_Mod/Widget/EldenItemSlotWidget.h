#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EldenItemSlotWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;
class UEldenItemDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemClickedDelegate, UEldenItemDefinition*, ItemDef, int32, SlotIndex);

UCLASS()
class ELDENRING_MOD_API UEldenItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemCountText;

	UPROPERTY(meta = (BindWidget))
	class UButton* ItemClickButton;

	UPROPERTY()
	class UEldenItemDefinition* DisplayedItem;

	UFUNCTION()
	void OnSlotClicked();

	int32 SlotIndex = -1;

public:
	void SetItemData(UEldenItemDefinition* Def, int32 Count, int32 InSlotIndex);

	UPROPERTY()
	FOnItemClickedDelegate OnSelectedItemClicked;
};

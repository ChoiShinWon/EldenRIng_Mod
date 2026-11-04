#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EldenItemActionPopupWidget.generated.h"

class UButton;
class UTextBlock;
class UEldenItemDefinition;

UENUM(BlueprintType)
enum class EItemActionType : uint8
{
	None UMETA(DisplayName = "None"),
	Equip UMETA(DisplayName = "Equip"),
	UnEquip UMETA(DisplayName = "UnEquip"),
	Use UMETA(DisplayName = "Use")
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnItemActionConfirmed, UEldenItemDefinition*, Item, EItemActionType, ActionType);

UCLASS()
class ELDENRING_MOD_API UEldenItemActionPopupWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* ActionButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ActionText;

	UPROPERTY()
	class UEldenItemDefinition* PopupItem = nullptr;

	UPROPERTY()
	EItemActionType PopupMode = EItemActionType::None;

	UFUNCTION()
	void ButtonClickedEvent();

public:
	void SetPopup(UEldenItemDefinition* Item, EItemActionType ActionType);

	UPROPERTY()
	FOnItemActionConfirmed OnActionConfirmed;
};

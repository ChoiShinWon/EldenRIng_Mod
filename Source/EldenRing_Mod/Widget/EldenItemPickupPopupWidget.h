#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EldenItemPickupPopupWidget.generated.h"

class UImage;
class UTextBlock;
class USoundBase;
class UWidgetAnimation;
class UEldenItemDefinition;

UCLASS()
class ELDENRING_MOD_API UEldenItemPickupPopupWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void ShowPopup(const UEldenItemDefinition* Def, int32 Amount, bool bIsNew);
	void HidePopup();

	bool IsShowing() const { return bShowing; }

protected:
	bool bShowing = false;

	UPROPERTY(meta = (BindWidget))
	UImage* Img_Icon = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_ItemName = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* Txt_Amount = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_NewTag = nullptr;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Anim_Show = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* AcquireSound = nullptr;

};

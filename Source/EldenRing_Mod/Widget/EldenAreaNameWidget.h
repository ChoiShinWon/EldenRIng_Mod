
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/TimerHandle.h"
#include "EldenAreaNameWidget.generated.h"

class UTextBlock;
class USoundBase;

UCLASS()
class ELDENRING_MOD_API UEldenAreaNameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 컨트롤러가 부르는 유일한 창구
	// 재생을 담당
	void ShowAreaName(const FText& Text, float Delay);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_AreaName = nullptr;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Anim_Show = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* ShowSound = nullptr;



	FTimerHandle ShowTimerHandle;

	FText PendingText;

	void PlayShow();
};

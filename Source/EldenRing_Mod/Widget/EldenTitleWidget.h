#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/TimerHandle.h"
#include "EldenTitleWidget.generated.h"

class UButton;
class UWorld;
class USoundBase;
class UAudioComponent;

UCLASS()
class ELDENRING_MOD_API UEldenTitleWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_NewGame = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Exit = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Title")
	TSoftObjectPtr<UWorld> NewGameLevel;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Anim_FadeOut = nullptr;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Anim_FadeIn = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Title")
	USoundBase* TitleBGM = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Title|Audio", meta = (ClampMin = "0.0"))
	float BGMStartDelay = 0.5f;

	UPROPERTY()
	UAudioComponent* BGMComponent = nullptr;

	FTimerHandle BGMTimerHandle;

	void StartBGM();

	UFUNCTION()
	void OnFadeInFinished();

	UFUNCTION()
	void OnFadeOutFinished();

	bool bIsTransitioning = false;

	UFUNCTION()
	void OnNewGameClicked();

	UFUNCTION()
	void OnExitClicked();
};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/TimerHandle.h"
#include "EldenTitleWidget.generated.h"


enum class ETitleMenuItem : uint8
{
	NewGame, Exit, Count,
	None = 255
};

class UButton;
class UWorld;
class USoundBase;
class UAudioComponent;
class UImage;
class UTextBlock;


UCLASS()
class ELDENRING_MOD_API UEldenTitleWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UImage* Img_SelectNewGame = nullptr;

	UPROPERTY(meta = (BindWidget))
	UImage* Img_SelectExit = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_NewGame = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Exit = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_NewGame = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Exit = nullptr;

	UPROPERTY()
	TArray<UImage*> SelectImages;

	UPROPERTY()
	TArray<UTextBlock*> Labels;

	UPROPERTY(EditDefaultsOnly, Category = "Color")
	FLinearColor SelectedColor = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, Category = "Color")
	FLinearColor NormalColor = FLinearColor::White;

	ETitleMenuItem CurrentItem = ETitleMenuItem::None;

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

	UFUNCTION()
	void OnNewGameHovered();

	UFUNCTION()
	void OnExitHovered();

	UFUNCTION()
	void OnMenuUnhovered();

	void SelectItem(ETitleMenuItem Item);

	void RefreshSelection();
};

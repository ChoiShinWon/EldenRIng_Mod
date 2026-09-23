#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EldenHUDWidget.generated.h"


class UProgressBar;
class UTextBlock;
class UImage;
class UTexture2D;

UCLASS()
class ELDENRING_MOD_API UEldenHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void UpdateEquipmentUI(UTexture2D* RTexture, UTexture2D* LTexture, UTexture2D* ItemTexture, const FString& SkillName);

	void ShowInteractPrompt(const FText& PromptText);
	void HideInteractPrompt();
protected:
	// 위젯이 화면에 생성될 때 한번 호출되는 함수
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;

	// 블루프린트의 위젯 이름과 반드시 일치해야 함!
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* GhostStaminaBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ManaBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* GhostManaBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* GhostHPBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RuneText;

	UPROPERTY(meta = (BindWidget))
	class UImage* RightIcon;

	UPROPERTY(meta = (BindWidget))
	class UImage* LeftIcon;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemIcon;

	UPROPERTY(meta = (BindWidget))
	class UImage* MagicIcon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CombatSkillText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PotionCountText;

	UPROPERTY(meta = (BindWidget))
	class UImage* InteractIcon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InteractPromptText;

	UPROPERTY(meta = (BindWidget))
	class UWidget* InteractPromptPanel;

	UFUNCTION()
	void OnRunesUpdated(int32 NewRunes);

	UFUNCTION()
	void OnHealthUpdated(float CurrentHealth, float MaxHealth);

	UFUNCTION()
	void OnManaUpdated(float CurrentMana, float MaxMana);

	UFUNCTION()
	void OnStaminaUpdated(float CurrentStamina, float MaxStamina);

	UFUNCTION()
	void OnSelectedItemChanged();

	UFUNCTION()
	void OnPotionCountUpdated(int32 Current, int32 Max);


	// 보간용 변수
	float GhostStaminaPercent = 1.0f;
	float GhostHPPercent = 1.0f;
	float GhostManaPercent = 1.0f;
    
	// 매 프레임 업데이트 (블루프린트의 Tick 역할)
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	// 캐릭터 참조 저장용
	UPROPERTY()
	class AEldenCharacter* PlayerRef;

	float TargetHPPercent = 1.0f;
	float TargetStaminaPercent = 1.0f;
	float TargetManaPercent = 1.0f;

	void UpdateStatBar(class UProgressBar* Bar, float& OutTargetPercent, float CurrentValue, float MaxValue);

	void TickGhostBar(class UProgressBar* GhostBarWidget, float& GhostPercent, float TargetPercent, float DeltaTime);
};

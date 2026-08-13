#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EldenHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
UCLASS()
class ELDENRING_MOD_API UEldenHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// 위젯이 화면에 생성될 때 한번 호출되는 함수
	virtual void NativeConstruct() override;

	// 블루프린트의 위젯 이름과 반드시 일치해야 함!
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* GhostBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* GhostHPBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RuneText;

	UFUNCTION()
	void OnRunesUpdated(int32 NewRunes);

	UFUNCTION()
	void OnHealthUpdated(float CurrentHealth, float MaxHealth);

	UFUNCTION()
	void OnStaminaUpdated(float CurrentStamina, float MaxStamina);

	// 보간용 변수
	float GhostPercent = 1.0f;
	float GhostHPPercent = 1.0f;
    
	// 매 프레임 업데이트 (블루프린트의 Tick 역할)
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	// 캐릭터 참조 저장용
	UPROPERTY()
	class AEldenCharacter* PlayerRef;

	float TargetHPPercent = 1.0f;
	float TargetStaminaPercent = 1.0f;
};
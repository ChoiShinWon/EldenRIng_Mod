#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "EldenEnemyHPBarComponent.generated.h"

class UEldenEnemyHPBarWidget;

UCLASS()
class ELDENRING_MOD_API UEldenEnemyHPBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UEldenEnemyHPBarComponent();


	void UpdateHealth(float CurrentHP, float MaxHP, bool bSnapGhost = false);
	// 최근 피격 on + 타이머 (재)시작
	void ShowOnHit();

	void SetLockedOn(bool bLocked);
	// 사망/리셋용
	void HideImmediately();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "HPBar", meta = (ClampMin = "0.1"))
	float ShowDuration = 4.f;

private:
	UPROPERTY()
	UEldenEnemyHPBarWidget* CachedHPBarWidget = nullptr;

	FTimerHandle HPBarHideTimerHandle;

	bool bLockedOn =false;
	bool bRecentlyHit = false;

	void RefreshVisibility();
	//최근 피격 off
	void ExpireHitDisplay();

	UEldenEnemyHPBarWidget* GetHPBarWidget();

};

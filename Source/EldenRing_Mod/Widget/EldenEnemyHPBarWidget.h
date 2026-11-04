#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EldenEnemyHPBarWidget.generated.h"

class UProgressBar;

UCLASS()
class ELDENRING_MOD_API UEldenEnemyHPBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateHealth(float CurrentHP, float MaxHP, bool bSnapGhost = false);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* GhostHPBar;

	UPROPERTY(EditDefaultsOnly, Category = Speed, meta = (ClampMin = "0.0"))
	float GhostInterpSpeed = 5.f;

private:
	float TargetHPPercent = 1.f;
	float GhostHPPercent = 1.f;
};

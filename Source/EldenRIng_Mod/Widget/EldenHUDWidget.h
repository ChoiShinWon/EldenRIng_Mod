#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EldenHUDWidget.generated.h"

UCLASS()
class ELDENRING_MOD_API UEldenHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// 블루프린트의 위젯 이름과 반드시 일치해야 함!
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* GhostBar;

	// 보간용 변수
	float GhostPercent = 1.0f;
    
	// 매 프레임 업데이트 (블루프린트의 Tick 역할)
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	// 캐릭터 참조 저장용
	UPROPERTY()
	class AEldenCharacter* PlayerRef;
};
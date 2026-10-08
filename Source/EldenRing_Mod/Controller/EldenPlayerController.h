#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EldenPlayerController.generated.h"

class UEldenHUDWidget;
class UUserWidget;

UCLASS()
class ELDENRING_MOD_API AEldenPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// EldenCharacter::BeginPlay에서 호출
	// 위젯 생성 + 뷰포트 추가 + 초기 장비 아이콘 동기화
	void InitHUD();

	// 은총 휴식 / 사망 / 부활에서 호출
	void SetHUDVisible(bool bVisible);

	// 장비 변경 시 EquipmentComponent가 호출
	void RefreshEquipmentUI();

	UEldenHUDWidget* GetCurrentHUD() const { return CurrentHUD; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY()
	UEldenHUDWidget* CurrentHUD = nullptr;
};

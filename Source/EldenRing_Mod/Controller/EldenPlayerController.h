#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EldenPlayerController.generated.h"

class UEldenHUDWidget;
class UEldenMenuWidget;
class UEldenAreaNameWidget;
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

	// HUD 접근용 getter 
	UEldenHUDWidget* GetCurrentHUD() const { return CurrentHUD; }

	void ToggleMenu();
	void OpenLevelUpMenu(TSubclassOf<UUserWidget> WidgetClass);

	void ExitUIMode();

	// 위젯에게 요청을 담당
	void ShowAreaName(const FText& Name, float Delay = 0.f);

	// 레벨 진입 자막 전용 진입점
	void ShowEntryAreaName();

protected:
	virtual void BeginPlay() override;

	// BP_EldenPlayerController에서 WBP_HUD 지정.
	// 비어 있으면 InitHUD가 조용히 return -> HUD가 안 뜸
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	// 멤버로 두는 이유: SetHUDVisible, GeCurrentHUD처럼 InitHUD 이후
	// 별도 호출에서도 접근해야 하기 때문
	UPROPERTY()
	UEldenHUDWidget* CurrentHUD = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> MenuWidgetClass;

	UPROPERTY()
	UEldenMenuWidget* MenuWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UEldenAreaNameWidget> AreaNameWidgetClass;

	UPROPERTY()
	UEldenAreaNameWidget* AreaWidget = nullptr;

	FText LastAreaName;

	// 시작 지역 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FText EntryAreaName;

	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (ClampMin = "0.0"))
	float EntryDelay = 0.f;

private:
	void EnterUIMode(UUserWidget* FocusWidget, bool bAllowGameInput);
};

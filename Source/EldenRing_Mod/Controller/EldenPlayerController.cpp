#include "EldenRing_Mod/Controller/EldenPlayerController.h"
#include "EldenRing_Mod/Widget/EldenHUDWidget.h"
#include "EldenRing_Mod/Widget/EldenMenuWidget.h"
#include "EldenRing_Mod/Widget/EldenAreaNameWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"


void AEldenPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ExitUIMode();
}


void AEldenPlayerController::InitHUD()
{
	// 클래스 미지정 or 이미 생성됨(중복 호출) -> 무시
	if (!HUDWidgetClass || CurrentHUD) return;

	// OwningPlayer는 반드시 this(PC). 폰을 넘기면 안됨
	CurrentHUD = CreateWidget<UEldenHUDWidget>(this, HUDWidgetClass);
	if (CurrentHUD)
	{
		CurrentHUD->AddToViewport();
	}
}

void AEldenPlayerController::SetHUDVisible(bool bVisible)
{
	if (CurrentHUD)
	{
		// Collapsed: 보이지 않을 뿐 아니라 레이아웃/입력에서도 완전히 제외
		CurrentHUD->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

}



void AEldenPlayerController::EnterUIMode(UUserWidget* FocusWidget, bool bAllowGameInput)
{
	bShowMouseCursor = true;

	if (bAllowGameInput)
	{
		// GameAndUI: UI가 먼저 입력을 받고, UI가 소비하지 않은 키는
		// 게임으로 전달됨
		FInputModeGameAndUI InputMode;
		// 멤버가 아니라 인자로 받은 위젯에 포커스
		// 대상 WBP는 IsFocusable이 켜져있어야 함
		InputMode.SetWidgetToFocus(FocusWidget->TakeWidget());
		InputMode.SetHideCursorDuringCapture(false);
		SetInputMode(InputMode);
	
	}
	else
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(FocusWidget->TakeWidget());
		SetInputMode(InputMode);
	}
}

void AEldenPlayerController::ExitUIMode()
{
	bShowMouseCursor = false;
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}


void AEldenPlayerController::OpenLevelUpMenu(TSubclassOf<UUserWidget> WidgetClass)
{
	if (!WidgetClass) return;
	UUserWidget* LevelUpWidget = CreateWidget<UUserWidget>(this, WidgetClass);
	if (!LevelUpWidget) return;

	LevelUpWidget->AddToViewport();
	EnterUIMode(LevelUpWidget, false);
	if (AEldenCharacter* PlayerCharacter = GetPawn<AEldenCharacter>())
	{
		PlayerCharacter->GetCharacterMovement()->StopMovementImmediately();
		PlayerCharacter->SetState(ECharacterState::Interacting);
	}
}

void AEldenPlayerController::ToggleMenu()
{
	AEldenCharacter* PlayerCharacter = GetPawn<AEldenCharacter>();
	if (!PlayerCharacter) return;
	// 닫기
	if (MenuWidget)
	{
		MenuWidget->RemoveFromParent();
		MenuWidget = nullptr;
		ExitUIMode();
		PlayerCharacter->SetState(ECharacterState::Idle);
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	}
	// 열기
	else
	{
		MenuWidget = CreateWidget<UEldenMenuWidget>(this, MenuWidgetClass);
		if (!MenuWidget) return;
		MenuWidget->InitMenu(PlayerCharacter->InventoryComponent, PlayerCharacter->StatComponent, PlayerCharacter->EquipmentComponent);
		MenuWidget->AddToViewport();
		EnterUIMode(MenuWidget, true);
		PlayerCharacter->GetCharacterMovement()->StopMovementImmediately();
		PlayerCharacter->SetState(ECharacterState::Interacting);
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0001f);
	}
}

void AEldenPlayerController::ShowAreaName(const FText& Name, float Delay)
{
	// 만약 마지막 방문한 곳과 이름이 같다면 무시
	if (LastAreaName.EqualTo(Name)) return;
	if (!AreaNameWidgetClass) return;

	// 위젯이 없다면 위젯 생성
	if (!AreaWidget)
	{
		AreaWidget = CreateWidget<UEldenAreaNameWidget>(this, AreaNameWidgetClass);
		// 생성했는데 없다면 return
		if (!AreaWidget) return;
		// 아니라면 뷰포트에 추가
		AreaWidget->AddToViewport(10);
	}

	// 마지막 방문한 지역 이름 갱신
	LastAreaName = Name;
	// 위젯에 재생 요청
	AreaWidget->ShowAreaName(Name, Delay);
}

void AEldenPlayerController::ShowEntryAreaName()
{
	if (EntryAreaName.IsEmpty()) return;
	ShowAreaName(EntryAreaName, EntryDelay);
}

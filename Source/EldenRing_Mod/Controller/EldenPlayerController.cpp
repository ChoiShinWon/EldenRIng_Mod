#include "EldenRing_Mod/Controller/EldenPlayerController.h"
#include "EldenRing_Mod/Widget/EldenHUDWidget.h"
#include "EldenRing_Mod/Widget/EldenMenuWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"



void AEldenPlayerController::InitHUD()
{
	if (!HUDWidgetClass || CurrentHUD) return;

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
		 CurrentHUD->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

}



void AEldenPlayerController::EnterUIMode(UUserWidget* FocusWidget, bool bAllowGameInput)
{
	bShowMouseCursor = true;

	if (bAllowGameInput)
	{
		FInputModeGameAndUI InputMode;
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

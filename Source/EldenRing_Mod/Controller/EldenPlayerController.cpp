#include "EldenRing_Mod/Controller/EldenPlayerController.h"
#include "EldenRing_Mod/Widget/EldenHUDWidget.h"
#include "EldenRing_Mod/Widget/EldenMenuWidget.h"
#include "EldenRing_Mod/Component/EldenEquipmentComponent.h"
#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Weapon/EldenWeapon.h"
#include "EldenRing_Mod/Weapon/EldenShield.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"



void AEldenPlayerController::InitHUD()
{
	if (!HUDWidgetClass || CurrentHUD) return;

	CurrentHUD = CreateWidget<UEldenHUDWidget>(this, HUDWidgetClass);
	if (CurrentHUD)
	{
		CurrentHUD->AddToViewport();
		RefreshEquipmentUI();
	}
}

void AEldenPlayerController::SetHUDVisible(bool bVisible)
{
	if (CurrentHUD)
	{
		 CurrentHUD->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

}

void AEldenPlayerController::RefreshEquipmentUI()
{
	if (!CurrentHUD) return;
	AEldenCharacter* PlayerCharacter = GetPawn<AEldenCharacter>();
	if (!PlayerCharacter) return;

	AEldenWeapon* Weapon = PlayerCharacter->EquipmentComponent->GetEquippedWeapon();
	AEldenShield* Shield = PlayerCharacter->EquipmentComponent->GetEquippedShield();
	
	UTexture2D* WeaponTexture = nullptr;
	UTexture2D* ShieldTexture = nullptr;
	FString CurrentSkillName = TEXT("");

	if (Weapon)
	{
		WeaponTexture = Weapon->GetIcon();
		CurrentSkillName = Weapon->GetSkillName();
	}
	// 방패를 장착하고 있는가가 아니라, 지금 화면에 방패가 보이는가를 기준으로 UI 갱신
	// EquippedShield 포인터 자체는 두손 무기 장착 중에도 계속 살아있음
	// SetActorHiddenInGame만 했지 슬롯에서 빼거나 nullptr로 비운게 아니기 때문
	// 포인터 유무만 따지면 두손 무기 장착 중에도 방패 UI가 보이기 때문에 IsHidden() 체크
	if (Shield && !Shield->IsHidden())
	{
		ShieldTexture = Shield->GetIcon();
		CurrentSkillName = Shield->GetSkillName();
	}
	CurrentHUD->UpdateEquipmentUI(WeaponTexture, ShieldTexture,
		PlayerCharacter->InventoryComponent->GetCurrentItemIcon(), CurrentSkillName);

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

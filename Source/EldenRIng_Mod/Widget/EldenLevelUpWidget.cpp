// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/Widget/EldenLevelUpWidget.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "EldenRing_Mod/Component/EldenGraceRestComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UEldenLevelUpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerCharacter = Cast<AEldenCharacter>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		PlayerStatComponent = PlayerCharacter->StatComponent;
		PendingVigor = PlayerStatComponent->Vigor;
		PendingEndurance = PlayerStatComponent->Endurance;
		PendingStrength = PlayerStatComponent->Strength;
		TotalPendingRuneCost = 0;
		UpdateRuneText();

		Txt_VigorValue->SetText(FText::AsNumber(PendingVigor));
		Txt_EnduranceValue->SetText(FText::AsNumber(PendingEndurance));
		Txt_StrengthValue->SetText(FText::AsNumber(PendingStrength));

		Txt_OriginalVigor->SetText(FText::AsNumber(PlayerStatComponent->Vigor));
		Txt_OriginalEndurance->SetText(FText::AsNumber(PlayerStatComponent->Endurance));
		Txt_OriginalStrength->SetText(FText::AsNumber(PlayerStatComponent->Strength));
	}


	if (Btn_VigorPlus)
	{
		Btn_VigorPlus->OnClicked.AddDynamic(this, &UEldenLevelUpWidget::OnVigorPlusClicked);
	}

	if (Btn_VigorMinus)
	{
		Btn_VigorMinus->OnClicked.AddDynamic(this, &UEldenLevelUpWidget::OnVigorMinusClicked);
	}

	if (Btn_EndurancePlus)
	{
		Btn_EndurancePlus->OnClicked.AddDynamic(this, &UEldenLevelUpWidget::OnEndurancePlusClicked);
	}

	if (Btn_EnduranceMinus)
	{
		Btn_EnduranceMinus->OnClicked.AddDynamic(this, &UEldenLevelUpWidget::OnEnduranceMinusClicked);
	}

	if (Btn_StrengthPlus)
	{
		Btn_StrengthPlus->OnClicked.AddDynamic(this, &UEldenLevelUpWidget::OnStrengthPlusClicked);
	}

	if (Btn_StrengthMinus)
	{
		Btn_StrengthMinus->OnClicked.AddDynamic(this, &UEldenLevelUpWidget::OnStrengthMinusClicked);
	}

	if (Btn_Confirm)
	{
		Btn_Confirm->OnClicked.AddDynamic(this, &UEldenLevelUpWidget::OnConfirmClicked);
	}

	if (Btn_Cancel)
	{
		Btn_Cancel->OnClicked.AddDynamic(this, &UEldenLevelUpWidget::OnCancelClicked);
	}
}

void UEldenLevelUpWidget::UpdateRuneText()
{
	if (PlayerStatComponent && Txt_LevelUpRunes)
	{
		int32 RemainRunes = PlayerStatComponent->CurrentRunes - TotalPendingRuneCost;
		Txt_LevelUpRunes->SetText(FText::AsNumber(RemainRunes));
	}
}


void UEldenLevelUpWidget::OnVigorPlusClicked()
{
	if (!PlayerCharacter || !PlayerStatComponent) return;
	TryChangeStat(PendingVigor, PlayerStatComponent->Vigor, Txt_VigorValue, +1);
	
}

void UEldenLevelUpWidget::OnVigorMinusClicked()
{
	if (!PlayerCharacter || !PlayerStatComponent) return;
	TryChangeStat(PendingVigor, PlayerStatComponent->Vigor, Txt_VigorValue, -1);
}

void UEldenLevelUpWidget::OnEndurancePlusClicked()
{
	if (!PlayerCharacter || !PlayerStatComponent) return;
	TryChangeStat(PendingEndurance, PlayerStatComponent->Endurance, Txt_EnduranceValue, +1);
	
}

void UEldenLevelUpWidget::OnEnduranceMinusClicked()
{
	if (!PlayerCharacter || !PlayerStatComponent) return;
	TryChangeStat(PendingEndurance, PlayerStatComponent->Endurance, Txt_EnduranceValue, -1);

	
}

void UEldenLevelUpWidget::OnStrengthPlusClicked()
{
	if (!PlayerCharacter || !PlayerStatComponent) return;
	TryChangeStat(PendingStrength, PlayerStatComponent->Strength, Txt_StrengthValue, +1);

}

void UEldenLevelUpWidget::OnStrengthMinusClicked()
{
	if (!PlayerCharacter || !PlayerStatComponent) return;
	TryChangeStat(PendingStrength, PlayerStatComponent->Strength, Txt_StrengthValue, -1);

}

void UEldenLevelUpWidget::OnConfirmClicked()
{
	if (PlayerCharacter && PlayerStatComponent)
	{
		int32 LevelGained = (PendingVigor - PlayerStatComponent->Vigor) +
			(PendingEndurance - PlayerStatComponent->Endurance) +
			(PendingStrength - PlayerStatComponent->Strength);

		PlayerStatComponent->Level += LevelGained;

		// 스탯 확정
		PlayerStatComponent->Vigor = PendingVigor;
		PlayerStatComponent->Endurance = PendingEndurance;
		PlayerStatComponent->Strength = PendingStrength;

		PlayerStatComponent->CurrentRunes -= TotalPendingRuneCost; // 진짜 결제

		PlayerStatComponent->OnRunesChanged.Broadcast(PlayerStatComponent->CurrentRunes);

		PlayerStatComponent->RecalculateDerivedStats();

		if (APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController()))
		{
			
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = false;
		}

		PlayerCharacter->GraceRestComponent->ExitRest();

		RemoveFromParent();
	}
}

void UEldenLevelUpWidget::OnCancelClicked()
{
	if (PlayerCharacter)
	{
		if (APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController()))
		{

			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = false;
		}

		PlayerCharacter->GraceRestComponent->ExitRest();
	}
	
	
	RemoveFromParent();
}


// Pending은 미결정된 값임. 참조로 받아서 원본을 실제로 바꿈
// BaseStat은 원래 스탯값
// ValueText 화면에 숫자를 띄워줄 텍스트 위젯
// Delta - Plus 면 +1, Minus면 -1. 증감 방향이랑 조건 분기를 결정
void UEldenLevelUpWidget::TryChangeStat(int32& PendingStat, int32 BaseStat, UTextBlock* ValueText, int32 Delta)
{
	int32 Cost = 500;
	bool bChanged = false;

	if (!PlayerStatComponent) return;

	if (Delta > 0)
	{
		if (PlayerStatComponent->CurrentRunes >= TotalPendingRuneCost + Cost)
		{
			PendingStat += Delta; // Delta가 +1이라 자동으로 증가
			TotalPendingRuneCost += Delta * Cost; // Delta가 +1이라 증가
			bChanged = true;
		}

	}

	if (Delta < 0)
	{
		if (PendingStat > BaseStat)
		{
			PendingStat += Delta; // Delta가 -1이라 자동으로 감소
			TotalPendingRuneCost += Delta * Cost; // Delta가 -1이라 자동으로 -500
			bChanged = true;
		}
	}

	if (bChanged && ValueText)
	{
		ValueText->SetText(FText::AsNumber(PendingStat));

		// 바꾼 후 값이 원래 값보다 크다면
		if (PendingStat > BaseStat)
		{
			// 파란색으로 색깔 변경
			ValueText->SetColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.6f, 1.0f)));
		}
		// 바꾼 후 값이 원래보다 작거나 같다면
		else
		{   // 하얀색으로 둔다
			ValueText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		}

		// 룬 텍스트 갱신
		UpdateRuneText();
	}
}

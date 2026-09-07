// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/Widget/EldenLevelUpWidget.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Component/EldenStatComponent.h"
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
	int32 Cost = 500;
	if (PlayerCharacter && PlayerStatComponent)
	{
		if (PlayerStatComponent->CurrentRunes >= TotalPendingRuneCost + Cost)
		{
			PendingVigor++;
			TotalPendingRuneCost += Cost;
			if (Txt_VigorValue)
			{
				Txt_VigorValue->SetText(FText::AsNumber(PendingVigor));
				if (PendingVigor > PlayerStatComponent->Vigor)
				{
					Txt_VigorValue->SetColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.6f, 1.0f)));
				}
			}
			UpdateRuneText();
		}

		else
		{
			// 나중에 룬 부족하다는 메시지 추가
		}
	}
	
	
}

void UEldenLevelUpWidget::OnVigorMinusClicked()
{
	int32 Cost = 500;
	if (PlayerCharacter && PlayerStatComponent)
	{
		if (PendingVigor > PlayerStatComponent->Vigor)
		{
			PendingVigor--;
			TotalPendingRuneCost -= Cost;

			if (Txt_VigorValue)
			{
				Txt_VigorValue->SetText(FText::AsNumber(PendingVigor));
				if (PendingVigor == PlayerStatComponent->Vigor)
				{
					Txt_VigorValue->SetColorAndOpacity(FSlateColor(FLinearColor::White));
				}
			}
			UpdateRuneText();
		}
	}
}

void UEldenLevelUpWidget::OnEndurancePlusClicked()
{
	int32 Cost = 500;
	if (PlayerCharacter && PlayerStatComponent)
	{
		if (PlayerStatComponent->CurrentRunes >= TotalPendingRuneCost + Cost)
		{
			PendingEndurance++;
			TotalPendingRuneCost += Cost;
			if (Txt_EnduranceValue)
			{
				Txt_EnduranceValue->SetText(FText::AsNumber(PendingEndurance));
				if (PendingEndurance > PlayerStatComponent->Endurance)
				{
					Txt_EnduranceValue->SetColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.6f, 1.0f)));
				}
			}
			UpdateRuneText();
		}
	}
	
}

void UEldenLevelUpWidget::OnEnduranceMinusClicked()
{
	int32 Cost = 500;
	if (PlayerStatComponent)
	{
		if (PlayerCharacter && PendingEndurance > PlayerStatComponent->Endurance)
		{
			PendingEndurance--;
			TotalPendingRuneCost -= Cost;
			if (Txt_EnduranceValue)
			{
				Txt_EnduranceValue->SetText(FText::AsNumber(PendingEndurance));
				if (PendingEndurance == PlayerStatComponent->Endurance)
				{
					Txt_EnduranceValue->SetColorAndOpacity(FSlateColor(FLinearColor::White));
				}
			}
			UpdateRuneText();
		}
	}
	
}

void UEldenLevelUpWidget::OnStrengthPlusClicked()
{
	int32 Cost = 500;
	if (PlayerCharacter && PlayerStatComponent)
	{
		if (PlayerStatComponent->CurrentRunes >= TotalPendingRuneCost + Cost)
		{
			PendingStrength++;
			TotalPendingRuneCost += Cost;
			if (Txt_StrengthValue)
			{
				Txt_StrengthValue->SetText(FText::AsNumber(PendingStrength));
				if (PendingStrength > PlayerStatComponent->Strength)
				{
					Txt_StrengthValue->SetColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.6f, 1.0f)));
				}
			}
			UpdateRuneText();

		}

	}
	
}

void UEldenLevelUpWidget::OnStrengthMinusClicked()
{
	int32 Cost = 500;
	if (PlayerCharacter && PendingStrength > PlayerStatComponent->Strength)
	{
		PendingStrength--;
		TotalPendingRuneCost -= Cost;
		if (Txt_StrengthValue)
		{
			Txt_StrengthValue->SetText(FText::AsNumber(PendingStrength));
			if (PendingStrength == PlayerStatComponent->Strength)
			{
				Txt_StrengthValue->SetColorAndOpacity(FSlateColor(FLinearColor::White));
			}
		}
		UpdateRuneText();
	}
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

		// 캐릭터 원래 상태로 복구 (이동 가능하게)
		PlayerCharacter->SetState(ECharacterState::Idle);

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
		// 캐릭터 원래 상태로 복구 (이동 가능하게)
		PlayerCharacter->SetState(ECharacterState::Idle);
	}
	
	
	RemoveFromParent();
}
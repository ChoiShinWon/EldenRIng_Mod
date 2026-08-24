#include "EldenHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "EldenRing_Mod/Character/EldenCharacter.h" 
#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EldenRing_Mod/StatUtils.h"

void UEldenHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
    PlayerRef = Cast<AEldenCharacter>(GetOwningPlayerPawn());
    if (PlayerRef && PlayerRef->StatComponent)
    {
        PlayerRef->StatComponent->OnHealthChanged.AddDynamic(this, &UEldenHUDWidget::OnHealthUpdated);
        PlayerRef->StatComponent->OnStaminaChanged.AddDynamic(this, &UEldenHUDWidget::OnStaminaUpdated);
        PlayerRef->StatComponent->OnRunesChanged.AddDynamic(this, &UEldenHUDWidget::OnRunesUpdated);

        OnHealthUpdated(PlayerRef->StatComponent->GetCurrentHealth(), PlayerRef->StatComponent->GetMaxHealth());
        OnStaminaUpdated(PlayerRef->StatComponent->GetCurrentStamina(), PlayerRef->StatComponent->GetMaxStamina());
        OnRunesUpdated(PlayerRef->StatComponent->CurrentRunes);
    }

}

void UEldenHUDWidget::OnRunesUpdated(int32 NewRunes)
{
    if (RuneText)
    {
        FString RuneStr = FString::FromInt(NewRunes);
        RuneText->SetText(FText::FromString(RuneStr));
    }

}

void UEldenHUDWidget::OnHealthUpdated(float CurrentHealth, float MaxHealth)
{
    if (MaxHealth > 0.0f)
    {
        TargetHPPercent = CurrentHealth / MaxHealth;
        if (HPBar) HPBar->SetPercent(TargetHPPercent);
    }
}

void UEldenHUDWidget::OnStaminaUpdated(float CurrentStamina, float MaxStamina)
{
    if (MaxStamina > 0.0f)
    {
        TargetStaminaPercent = CurrentStamina / MaxStamina;
        if (StaminaBar) StaminaBar->SetPercent(TargetStaminaPercent);
    }
}


void UEldenHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    //  StatComponent가 있는지 확인
    if (PlayerRef && PlayerRef->StatComponent && StaminaBar && GhostBar && HPBar && GhostHPBar)
    {
        

        // 노란색 바 보간
        GhostPercent = FStatUtils::InterpGhostValue(GhostPercent, TargetStaminaPercent, InDeltaTime, 5.0f);
        GhostBar->SetPercent(GhostPercent);

        

        GhostHPPercent = FStatUtils::InterpGhostValue(GhostHPPercent, TargetHPPercent, InDeltaTime, 5.0f);
        GhostHPBar->SetPercent(GhostHPPercent);
    }
}


void UEldenHUDWidget::UpdateEquipmentUI(UTexture2D* RTexture, UTexture2D* LTexture, UTexture2D* ItemTexture, const FString& SkillName)
{
    if (RightIcon)
    {
        if (RTexture)
        {
            RightIcon->SetBrushFromTexture(RTexture);
            RightIcon->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            RightIcon->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    if (LeftIcon)
    {
        if (LTexture)
        {
            LeftIcon->SetBrushFromTexture(LTexture);
            LeftIcon->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            LeftIcon->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    if (ItemIcon)
    {
        if (ItemTexture)
        {
            ItemIcon->SetBrushFromTexture(ItemTexture);
            ItemIcon->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            ItemIcon->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    if (MagicIcon)
    {
        MagicIcon->SetVisibility(ESlateVisibility::Hidden);
    }

    if (CombatSkillText)
    {
        if (SkillName.IsEmpty())
        {
            CombatSkillText->SetVisibility(ESlateVisibility::Hidden);
        }

        else
        {
            CombatSkillText->SetText(FText::FromString(SkillName));
            CombatSkillText->SetVisibility(ESlateVisibility::Visible);
        }
    }
}

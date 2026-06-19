#include "EldenHUDWidget.h"
#include "Components/ProgressBar.h"
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
    
        OnHealthUpdated(PlayerRef->StatComponent->GetCurrentHealth(), PlayerRef->StatComponent->GetMaxHealth());
        OnStaminaUpdated(PlayerRef->StatComponent->GetCurrentStamina(), PlayerRef->StatComponent->GetMaxStamina());
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


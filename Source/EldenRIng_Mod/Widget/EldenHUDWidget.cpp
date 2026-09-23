#include "EldenHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "EldenRing_Mod/Character/EldenCharacter.h" 
#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EldenRing_Mod/StatUtils.h"

void UEldenHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
    PlayerRef = Cast<AEldenCharacter>(GetOwningPlayerPawn());
    if (!PlayerRef) return;
    if (PlayerRef->StatComponent)
    {
        PlayerRef->StatComponent->OnHealthChanged.AddDynamic(this, &UEldenHUDWidget::OnHealthUpdated);
        PlayerRef->StatComponent->OnStaminaChanged.AddDynamic(this, &UEldenHUDWidget::OnStaminaUpdated);
        PlayerRef->StatComponent->OnRunesChanged.AddDynamic(this, &UEldenHUDWidget::OnRunesUpdated);
		PlayerRef->StatComponent->OnManaChanged.AddDynamic(this, &UEldenHUDWidget::OnManaUpdated);

        OnHealthUpdated(PlayerRef->StatComponent->GetCurrentHealth(), PlayerRef->StatComponent->GetMaxHealth());
        OnStaminaUpdated(PlayerRef->StatComponent->GetCurrentStamina(), PlayerRef->StatComponent->GetMaxStamina());
        OnRunesUpdated(PlayerRef->StatComponent->CurrentRunes);
		OnManaUpdated(PlayerRef->StatComponent->GetCurrentMana(), PlayerRef->StatComponent->GetMaxMana());
       

    }
    if (PlayerRef->InventoryComponent)
    {
		PlayerRef->InventoryComponent->OnSelectedItemChanged.AddDynamic(this,&UEldenHUDWidget::OnSelectedItemChanged);
        PlayerRef->InventoryComponent->OnPotionCountChanged.AddDynamic(this, &UEldenHUDWidget::OnPotionCountUpdated);

		OnSelectedItemChanged();
        OnPotionCountUpdated(PlayerRef->InventoryComponent->GetCurrentPotionCount(), PlayerRef->InventoryComponent->GetMaxPotionCount());
    }

}

void UEldenHUDWidget::NativeDestruct()
{
    if (PlayerRef && PlayerRef->StatComponent)
    {
        PlayerRef->StatComponent->OnHealthChanged.RemoveDynamic(this, &UEldenHUDWidget::OnHealthUpdated);
        PlayerRef->StatComponent->OnStaminaChanged.RemoveDynamic(this, &UEldenHUDWidget::OnStaminaUpdated);
        PlayerRef->StatComponent->OnRunesChanged.RemoveDynamic(this, &UEldenHUDWidget::OnRunesUpdated);
		PlayerRef->StatComponent->OnManaChanged.RemoveDynamic(this, &UEldenHUDWidget::OnManaUpdated);
	}

    if (PlayerRef && PlayerRef->InventoryComponent)
    {
		PlayerRef->InventoryComponent->OnSelectedItemChanged.RemoveDynamic(this, &UEldenHUDWidget::OnSelectedItemChanged);
        PlayerRef->InventoryComponent->OnPotionCountChanged.RemoveDynamic(this, &UEldenHUDWidget::OnPotionCountUpdated);
    
    }
    Super::NativeDestruct();
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
	UpdateStatBar(HPBar, TargetHPPercent, CurrentHealth, MaxHealth);
}

void UEldenHUDWidget::OnManaUpdated(float CurrentMana, float MaxMana)
{
	UpdateStatBar(ManaBar, TargetManaPercent, CurrentMana, MaxMana);
}

void UEldenHUDWidget::OnStaminaUpdated(float CurrentStamina, float MaxStamina)
{
	UpdateStatBar(StaminaBar, TargetStaminaPercent, CurrentStamina, MaxStamina);
}

void UEldenHUDWidget::OnSelectedItemChanged()
{
	if (!PlayerRef || !PlayerRef->InventoryComponent || !ItemIcon) return;

	UTexture2D* Icon = PlayerRef->InventoryComponent->GetCurrentItemIcon();
	if (Icon)
	{
		ItemIcon->SetBrushFromTexture(Icon);
		ItemIcon->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ItemIcon->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UEldenHUDWidget::OnPotionCountUpdated(int32 Current, int32 Max)
{
    if (PotionCountText)
    {
        FString PotionStr = FString::FromInt(Current);
        PotionCountText->SetText(FText::FromString(PotionStr));
    }
}


void UEldenHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    //  StatComponent가 있는지 확인
    if (PlayerRef && PlayerRef->StatComponent && StaminaBar && GhostStaminaBar && HPBar && GhostHPBar && ManaBar && GhostManaBar)
    {
		TickGhostBar(GhostStaminaBar, GhostStaminaPercent, TargetStaminaPercent, InDeltaTime);
		TickGhostBar(GhostHPBar, GhostHPPercent, TargetHPPercent, InDeltaTime);
		TickGhostBar(GhostManaBar, GhostManaPercent, TargetManaPercent, InDeltaTime);
    }
}

void UEldenHUDWidget::UpdateStatBar(UProgressBar* Bar, float& OutTargetPercent, float CurrentValue, float MaxValue)
{
	if (MaxValue <= 0) return;
	OutTargetPercent = CurrentValue / MaxValue;
	if (Bar) Bar->SetPercent(OutTargetPercent);
}

void UEldenHUDWidget::TickGhostBar(UProgressBar* GhostBarWidget, float& GhostPercent, float TargetPercent, float DeltaTime)
{
	if (!GhostBarWidget) return;
	GhostPercent = FStatUtils::InterpGhostValue(GhostPercent, TargetPercent, DeltaTime, 5.0f);
	GhostBarWidget->SetPercent(GhostPercent);
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

void UEldenHUDWidget::ShowInteractPrompt(const FText& PromptText)
{
	InteractPromptText->SetText(PromptText);
	InteractPromptPanel->SetVisibility(ESlateVisibility::Visible);
}

void UEldenHUDWidget::HideInteractPrompt()
{
	InteractPromptPanel->SetVisibility(ESlateVisibility::Collapsed);
}

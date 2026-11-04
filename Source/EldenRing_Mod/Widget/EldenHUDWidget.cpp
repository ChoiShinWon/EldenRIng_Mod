#include "EldenHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "EldenRing_Mod/Weapon/EldenWeapon.h"
#include "EldenRing_Mod/Weapon/EldenShield.h"
#include "EldenRing_Mod/Character/EldenCharacter.h" 
#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "EldenRing_Mod/Component/EldenEquipmentComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EldenRing_Mod/StatUtils.h"

void UEldenHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
	if (Anim_FadeIn)
	{
		PlayAnimation(Anim_FadeIn);
	}

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
        PlayerRef->InventoryComponent->OnSelectedHotbarItemCountChanged.AddDynamic(this, &UEldenHUDWidget::OnSelectedHotbarItemCountUpdated);

		OnSelectedItemChanged();
        OnSelectedHotbarItemCountUpdated(PlayerRef->InventoryComponent->GetCurrentItemCount(), PlayerRef->InventoryComponent->GetMaxItemCount());
    }

	if (PlayerRef->EquipmentComponent)
	{
		PlayerRef->EquipmentComponent->OnEquipmentChanged.AddDynamic(this, &UEldenHUDWidget::OnEquipmentChanged);
		OnEquipmentChanged();
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
        PlayerRef->InventoryComponent->OnSelectedHotbarItemCountChanged.RemoveDynamic(this, &UEldenHUDWidget::OnSelectedHotbarItemCountUpdated);
    
    }

	if (PlayerRef && PlayerRef->EquipmentComponent)
	{
		PlayerRef->EquipmentComponent->OnEquipmentChanged.RemoveDynamic(this, &UEldenHUDWidget::OnEquipmentChanged);
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

// 핫바에서 "선택된" 아이템의 (현재, 최대) 수량 방송을 받아 수량 텍스트를 갱신
// - 어떤 아이템이든 선택된 것의 수량을 그대로 표시 (HP/마나 포션, 일반 소모품 공통)
// - Max는 지금 쓰지 않음: MaxCount가 1인 소모품도 "1"로 표시됨. 숨기고 싶으면 여기서 Max > 1 조건 추가
// - 위젯 변수명 PotionCountText는 WBP의 BindWidget 이름과 맞아야 해서 일부러 안 바꿨음
void UEldenHUDWidget::OnSelectedHotbarItemCountUpdated(int32 Current, int32 Max)
{
    if (PotionCountText)
    {
        FString PotionStr = FString::FromInt(Current);
        PotionCountText->SetText(FText::FromString(PotionStr));
    }
}

void UEldenHUDWidget::OnEquipmentChanged()
{
	if (!PlayerRef) return;

	AEldenWeapon* Weapon = PlayerRef->EquipmentComponent->GetEquippedWeapon();
	AEldenShield* Shield = PlayerRef->EquipmentComponent->GetEquippedShield();

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
	UpdateEquipmentUI(WeaponTexture, ShieldTexture, CurrentSkillName);
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


void UEldenHUDWidget::UpdateEquipmentUI(UTexture2D* RTexture, UTexture2D* LTexture, const FString& SkillName)
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

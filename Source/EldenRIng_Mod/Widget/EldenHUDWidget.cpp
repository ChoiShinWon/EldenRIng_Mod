#include "EldenHUDWidget.h"
#include "Components/ProgressBar.h"
#include "EldenRing_Mod/Character/EldenCharacter.h" 
#include "Kismet/KismetMathLibrary.h"
#include "EldenRing_Mod/StatUtils.h"

void UEldenHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (PlayerRef == nullptr)
	{
		PlayerRef = Cast<AEldenCharacter>(GetOwningPlayerPawn());
	}

	if (PlayerRef && StaminaBar && GhostBar && HPBar && GhostHPBar)
	{
		// Getter 함수 사용 
		float TargetStamina = PlayerRef->GetCurrentStamina() / PlayerRef->GetMaxStamina();
		StaminaBar->SetPercent(TargetStamina);
		// 노란색 바 보간
		GhostPercent = FStatUtils::InterpGhostValue(GhostPercent, TargetStamina, InDeltaTime, 5.0f);
		GhostBar->SetPercent(GhostPercent);

		float TargetHP = PlayerRef->GetCurrentHealth() / PlayerRef->GetMaxHealth();
		HPBar->SetPercent(TargetHP);

		GhostHPPercent = FStatUtils::InterpGhostValue(GhostHPPercent, TargetHP, InDeltaTime, 5.0f);
		GhostHPBar->SetPercent(GhostHPPercent);
	}
}
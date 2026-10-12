#include "EldenRing_Mod/Widget/EldenEnemyHPBarWidget.h"
#include "Components/ProgressBar.h"
#include "EldenRing_Mod/StatUtils.h"


void UEldenEnemyHPBarWidget::UpdateHealth(float CurrentHP, float MaxHP, bool bSnapGhost)
{
	if (MaxHP <= 0) return;
	TargetHPPercent = CurrentHP / MaxHP;

	if (HPBar)
	{
		HPBar->SetPercent(TargetHPPercent);
	}
	if (bSnapGhost)
	{
		GhostHPPercent = TargetHPPercent;
		if (GhostHPBar) GhostHPBar->SetPercent(GhostHPPercent);
	}
}



void UEldenEnemyHPBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!GhostHPBar) return;
	GhostHPPercent = FStatUtils::InterpGhostValue(GhostHPPercent, TargetHPPercent, InDeltaTime, GhostInterpSpeed);
	GhostHPBar->SetPercent(GhostHPPercent);

}

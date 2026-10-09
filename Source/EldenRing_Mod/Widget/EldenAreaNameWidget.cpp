#include "EldenRing_Mod/Widget/EldenAreaNameWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UEldenAreaNameWidget::ShowAreaName(const FText& Text, float Delay)
{
	if (!GetWorld()) return;
	GetWorld()->GetTimerManager().ClearTimer(ShowTimerHandle);
	PendingText = Text;

	if (Delay > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(ShowTimerHandle,
			this,
			&UEldenAreaNameWidget::PlayShow,
			Delay,
			false);
	}
	else
	{
		PlayShow();	
	}
}

void UEldenAreaNameWidget::PlayShow()
{
	Txt_AreaName->SetText(PendingText);
	PlayAnimation(Anim_Show);
	if (ShowSound)
	{
		UGameplayStatics::PlaySound2D(this, ShowSound);
	}
}

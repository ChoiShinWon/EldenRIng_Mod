
#include "EldenRing_Mod/Widget/EldenTitleWidget.h"
#include "Components/AudioComponent.h"
#include "Components/Button.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UEldenTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_NewGame->OnClicked.AddDynamic(this, &UEldenTitleWidget::OnNewGameClicked);
	Btn_Exit->OnClicked.AddDynamic(this, &UEldenTitleWidget::OnExitClicked);

	FWidgetAnimationDynamicEvent FinishedDelegate;
	FinishedDelegate.BindDynamic(this, &UEldenTitleWidget::OnFadeOutFinished);
	BindToAnimationFinished(Anim_FadeOut, FinishedDelegate);

	FWidgetAnimationDynamicEvent FadeInFinishedDelegate;
	FadeInFinishedDelegate.BindDynamic(this, &UEldenTitleWidget::OnFadeInFinished);
	BindToAnimationFinished(Anim_FadeIn, FadeInFinishedDelegate);
	bIsTransitioning = true;
	PlayAnimation(Anim_FadeIn);

	if(BGMStartDelay > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(BGMTimerHandle, this, &UEldenTitleWidget::StartBGM, BGMStartDelay, false);
	}
	else
	{
		StartBGM();
	}

}



void UEldenTitleWidget::OnNewGameClicked()
{
	if (NewGameLevel.IsNull()) return;

	if (bIsTransitioning) return;
	bIsTransitioning = true;
	GetWorld()->GetTimerManager().ClearTimer(BGMTimerHandle);
	Btn_NewGame->SetIsEnabled(false);
	Btn_Exit->SetIsEnabled(false);
	PlayAnimation(Anim_FadeOut);

	if (IsValid(BGMComponent))
	{
		BGMComponent->FadeOut(Anim_FadeOut->GetEndTime(), 0.f);
	}
}

void UEldenTitleWidget::OnExitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UEldenTitleWidget::StartBGM()
{
	if (!TitleBGM) return;

	BGMComponent = UGameplayStatics::CreateSound2D(this, TitleBGM);
	if (BGMComponent)
	{
		BGMComponent->FadeIn(1.5f);
	}
}

void UEldenTitleWidget::OnFadeInFinished()
{
	bIsTransitioning = false;
}

void UEldenTitleWidget::OnFadeOutFinished()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, NewGameLevel);
}

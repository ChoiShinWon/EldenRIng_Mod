
#include "EldenRing_Mod/Widget/EldenTitleWidget.h"
#include "Components/AudioComponent.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UEldenTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SelectImages.Reset();
	Labels.Reset();

	SelectImages.Add(Img_SelectNewGame);
	Labels.Add(Txt_NewGame);
	SelectImages.Add(Img_SelectExit);
	Labels.Add(Txt_Exit);

	Btn_NewGame->OnClicked.AddDynamic(this, &UEldenTitleWidget::OnNewGameClicked);
	Btn_Exit->OnClicked.AddDynamic(this, &UEldenTitleWidget::OnExitClicked);
	Btn_NewGame->OnHovered.AddDynamic(this, &UEldenTitleWidget::OnNewGameHovered);
	Btn_Exit->OnHovered.AddDynamic(this, &UEldenTitleWidget::OnExitHovered);
	Btn_NewGame->OnUnhovered.AddDynamic(this, &UEldenTitleWidget::OnMenuUnhovered);
	Btn_Exit->OnUnhovered.AddDynamic(this, &UEldenTitleWidget::OnMenuUnhovered);


	RefreshSelection();

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

void UEldenTitleWidget::OnNewGameHovered()
{
	SelectItem(ETitleMenuItem::NewGame);
}

void UEldenTitleWidget::OnExitHovered()
{
	SelectItem(ETitleMenuItem::Exit);
}

void UEldenTitleWidget::OnMenuUnhovered()
{
	SelectItem(ETitleMenuItem::None);
}

void UEldenTitleWidget::SelectItem(ETitleMenuItem Item)
{
	if (CurrentItem == Item) return;
	CurrentItem = Item;
	RefreshSelection();

}

void UEldenTitleWidget::RefreshSelection()
{
	const int32 Count = static_cast<int32>(ETitleMenuItem::Count);
	if (SelectImages.Num() != Count || Labels.Num() != Count) return;

	for (int32 i = 0; i < Count; ++i)
	{
		const bool bSelected = (i == static_cast<int32>(CurrentItem));

		SelectImages[i]->SetVisibility(bSelected ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
		Labels[i]->SetColorAndOpacity(FSlateColor(bSelected ? SelectedColor : NormalColor));
	}

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

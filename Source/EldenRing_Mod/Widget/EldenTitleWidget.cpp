
#include "EldenRing_Mod/Widget/EldenTitleWidget.h"
#include "EldenRing_Mod/Save/EldenSaveSubsystem.h"
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

	SelectImages.Add(Img_SelectContinue);
	Labels.Add(Txt_Continue);
	SelectImages.Add(Img_SelectNewGame);
	Labels.Add(Txt_NewGame);
	SelectImages.Add(Img_SelectExit);
	Labels.Add(Txt_Exit);

	Btn_Continue->OnClicked.AddDynamic(this, &UEldenTitleWidget::OnContinueClicked);
	Btn_NewGame->OnClicked.AddDynamic(this, &UEldenTitleWidget::OnNewGameClicked);
	Btn_Exit->OnClicked.AddDynamic(this, &UEldenTitleWidget::OnExitClicked);

	Btn_Continue->OnHovered.AddDynamic(this, &UEldenTitleWidget::OnContinueHovered);
	Btn_NewGame->OnHovered.AddDynamic(this, &UEldenTitleWidget::OnNewGameHovered);
	Btn_Exit->OnHovered.AddDynamic(this, &UEldenTitleWidget::OnExitHovered);

	Btn_Continue->OnUnhovered.AddDynamic(this, &UEldenTitleWidget::OnMenuUnhovered);
	Btn_NewGame->OnUnhovered.AddDynamic(this, &UEldenTitleWidget::OnMenuUnhovered);
	Btn_Exit->OnUnhovered.AddDynamic(this, &UEldenTitleWidget::OnMenuUnhovered);

	Sub = GetWorld()->GetGameInstance()->GetSubsystem<UEldenSaveSubsystem>();
	SetContinueAvailable(Sub && Sub->HasSave());

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

void UEldenTitleWidget::OnContinueClicked()
{
	if (NewGameLevel.IsNull() || bIsTransitioning) return;
	if (!Sub) return;

	if (!Sub->LoadFromDisk())
	{
		SetContinueAvailable(false);
		return;
	}
	StartTransition();
	
}

void UEldenTitleWidget::OnNewGameClicked()
{
	if (bIsTransitioning) return;
	if (NewGameLevel.IsNull()) return;
	if (!Sub) return;
	Sub->ClearCurrentSave();

	StartTransition();
}

void UEldenTitleWidget::StartTransition()
{
	if (bIsTransitioning) return;
	bIsTransitioning = true;
	GetWorld()->GetTimerManager().ClearTimer(BGMTimerHandle);
	Btn_Continue->SetIsEnabled(false);
	Btn_NewGame->SetIsEnabled(false);
	Btn_Exit->SetIsEnabled(false);
	PlayAnimation(Anim_FadeOut);

	if (IsValid(BGMComponent))
	{
		BGMComponent->FadeOut(Anim_FadeOut->GetEndTime(), 0.f);
	}
}

void UEldenTitleWidget::SetContinueAvailable(bool bAvailable)
{
	if (bAvailable) Row_Continue->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	else Row_Continue->SetVisibility(ESlateVisibility::Collapsed);
}

void UEldenTitleWidget::OnExitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UEldenTitleWidget::OnContinueHovered()
{
	SelectItem(ETitleMenuItem::Continue);
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

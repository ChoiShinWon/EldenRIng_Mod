#include "EldenRing_Mod/Component/EldenEnemyHPBarComponent.h"
#include "EldenRing_Mod/Widget/EldenEnemyHPBarWidget.h"
#include "Engine/World.h"

UEldenEnemyHPBarComponent::UEldenEnemyHPBarComponent()
{
	SetWidgetSpace(EWidgetSpace::Screen);
	SetVisibility(false);
	SetDrawSize(FVector2D(140, 8));
}

UEldenEnemyHPBarWidget* UEldenEnemyHPBarComponent::GetHPBarWidget()
{
	if (!CachedHPBarWidget)
	{
		CachedHPBarWidget = Cast<UEldenEnemyHPBarWidget>(GetUserWidgetObject());
	}
	return CachedHPBarWidget;
}

void UEldenEnemyHPBarComponent::UpdateHealth(float CurrentHP, float MaxHP, bool bSnapGhost)
{
	UEldenEnemyHPBarWidget* HPBar = GetHPBarWidget();
	if (!HPBar) return;
	HPBar->UpdateHealth(CurrentHP, MaxHP, bSnapGhost);
}

void UEldenEnemyHPBarComponent::ShowOnHit()
{
	bRecentlyHit = true;
	GetWorld()->GetTimerManager().SetTimer(
		HPBarHideTimerHandle,
		this,
		&UEldenEnemyHPBarComponent::ExpireHitDisplay,
		ShowDuration,
		false);
	RefreshVisibility();
}

void UEldenEnemyHPBarComponent::SetLockedOn(bool bLocked)
{
	bLockedOn = bLocked;
	RefreshVisibility();
}

void UEldenEnemyHPBarComponent::HideImmediately()
{
	bLockedOn = false;
	bRecentlyHit = false;
	GetWorld()->GetTimerManager().ClearTimer(HPBarHideTimerHandle);
	RefreshVisibility();
}

void UEldenEnemyHPBarComponent::RefreshVisibility()
{
	SetVisibility(bLockedOn || bRecentlyHit);
}

void UEldenEnemyHPBarComponent::ExpireHitDisplay()
{
	bRecentlyHit = false;
	RefreshVisibility();
}

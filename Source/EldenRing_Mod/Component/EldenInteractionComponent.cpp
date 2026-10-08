#include "EldenRing_Mod/Component/EldenInteractionComponent.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Controller/EldenPlayerController.h"
#include "EldenRing_Mod/Widget/EldenHUDWidget.h"

UEldenInteractionComponent::UEldenInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UEldenInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<AEldenCharacter>(GetOwner());

	
}

void UEldenInteractionComponent::SetInteractableTarget(TScriptInterface<class IInteractable> NewTarget)
{
	CurrentInteractableTarget = NewTarget;

	AEldenPlayerController* PC = PlayerCharacter->GetEldenController();
	if (!PC) return;
	UEldenHUDWidget* CurrentHUD = PC->GetCurrentHUD();
	if (!CurrentHUD) return;
	if (NewTarget)
	{
		CurrentHUD->ShowInteractPrompt(NewTarget->GetInteractionPrompt());
	}
	else
	{
		CurrentHUD->HideInteractPrompt();
	}
}

void UEldenInteractionComponent::ExecuteInteract()
{
	if (CurrentInteractableTarget != nullptr)
	{
		CurrentInteractableTarget->Interact(PlayerCharacter);
	}
}

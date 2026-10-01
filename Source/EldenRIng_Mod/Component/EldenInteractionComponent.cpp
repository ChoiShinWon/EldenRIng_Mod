#include "EldenRing_Mod/Component/EldenInteractionComponent.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
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

	if (!PlayerCharacter->GetCurrentHUD()) return;
	if (NewTarget)
	{
		PlayerCharacter->GetCurrentHUD()->ShowInteractPrompt(NewTarget->GetInteractionPrompt());
	}
	else
	{
		PlayerCharacter->GetCurrentHUD()->HideInteractPrompt();
	}
}

void UEldenInteractionComponent::ExecuteInteract()
{
	if (CurrentInteractableTarget != nullptr)
	{
		CurrentInteractableTarget->Interact(PlayerCharacter);
	}
}

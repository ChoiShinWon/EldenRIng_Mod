
#include "EldenRing_Mod/Actor/EldenNPC.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Controller/EldenPlayerController.h"
#include "EldenRing_Mod/Component/EldenInteractionComponent.h"
#include "EldenRing_Mod/Dialogue/EldenDialogueData.h"

AEldenNPC::AEldenNPC()
{
	PrimaryActorTick.bCanEverTick = false;
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	RootComponent = InteractionSphere;
	NPCMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("NPCMesh"));
	NPCMesh->SetupAttachment(RootComponent);

	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEldenNPC::OnOverlapBegin);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AEldenNPC::OnOverlapEnd);
}

void AEldenNPC::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (AEldenCharacter* Player = Cast<AEldenCharacter>(OtherActor))
	{
		Player->InteractionComponent->SetInteractableTarget(this);
	}
}

void AEldenNPC::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AEldenCharacter* Player = Cast<AEldenCharacter>(OtherActor))
	{
		if (bTalking && CurrentPlayer.Get() == Player)
		{
			EndDialogue();
		}
		Player->InteractionComponent->SetInteractableTarget(nullptr);
	}
	
}

FText AEldenNPC::GetInteractionPrompt() const
{
	return FText::FromString(TEXT("대화하기"));
}

void AEldenNPC::Interact(AEldenCharacter* Player)
{
	if (!Player) return;
	if (!bTalking)
	{
		if (Player->GetState() != ECharacterState::Idle) return;
		StartDialogue(Player);
	}
	else
	{
		AdvanceDialogue();
	}
}

void AEldenNPC::StartDialogue(AEldenCharacter* Player)
{
	if (!DialogueData) return;
	bTalking = true;
	CurrentLineIndex = 0;
	CurrentPlayer = Player;

	Player->GetCharacterMovement()->StopMovementImmediately();
	Player->SetState(ECharacterState::Interacting);
	if (Player->GetEldenController())
	{
		Player->GetEldenController()->ShowDialogue(DialogueData->GetSpeakerName(), DialogueData->GetLine(CurrentLineIndex));
	}
}

void AEldenNPC::AdvanceDialogue()
{
	if (DialogueData->IsLastLine(CurrentLineIndex))
	{
		EndDialogue();
		return;
	}
	++CurrentLineIndex;

	if (AEldenPlayerController* PC = GetCurrentPC())
	{
		PC->ShowDialogue(DialogueData->GetSpeakerName(), DialogueData->GetLine(CurrentLineIndex));
	}
	
}

void AEldenNPC::EndDialogue()
{
	if (!bTalking) return;
	bTalking = false;
	CurrentLineIndex = 0;
	if (CurrentPlayer.IsValid())
	{
		if (AEldenPlayerController* PC = GetCurrentPC())
		{
			PC->HideDialogue();
		}
		
		if (CurrentPlayer->GetState() == ECharacterState::Interacting)
		{
			CurrentPlayer->SetState(ECharacterState::Idle);
		}
	}
	CurrentPlayer.Reset();

}

AEldenPlayerController* AEldenNPC::GetCurrentPC() const
{
	if (!CurrentPlayer.IsValid()) return nullptr;
	return CurrentPlayer->GetEldenController();
}

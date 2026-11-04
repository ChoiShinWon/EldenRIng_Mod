
#include "EldenRing_Mod/Actor/EldenNPC.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EldenRing_Mod/EldenGameMode.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Controller/EldenPlayerController.h"
#include "EldenRing_Mod/Component/EldenInteractionComponent.h"
#include "EldenRing_Mod/Component/EldenStatComponent.h"
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



void AEldenNPC::BeginPlay()
{
	Super::BeginPlay();
	if (AEldenGameMode* GameMode = Cast<AEldenGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->OnEnemyKilled.AddDynamic(this, &AEldenNPC::HandleEnemyKilled);
		GameMode->OnGraceRested.AddDynamic(this, &AEldenNPC::HandleGraceRested);
	}
}

void AEldenNPC::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AEldenGameMode* GameMode = Cast<AEldenGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->OnEnemyKilled.RemoveDynamic(this, &AEldenNPC::HandleEnemyKilled);
		GameMode->OnGraceRested.RemoveDynamic(this, &AEldenNPC::HandleGraceRested);

	}
	Super::EndPlay(EndPlayReason);
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
		Player->InteractionComponent->ClearInteractableTarget(this);
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

void AEldenNPC::HandleEnemyKilled()
{
	if (!Stages.IsValidIndex(CurrentStage)) return;
	
	const FNPCStage& Stage = Stages[CurrentStage];
	if (Stage.Advance != ENPCStageAdvance::OnKillTarget) return;
	if (++KillCount >= Stage.KillTarget) AdvanceStage();
	
}

void AEldenNPC::HandleGraceRested()
{
	if (Stages.IsValidIndex(CurrentStage) && Stages[CurrentStage].Advance == ENPCStageAdvance::OnGraceRest)
	{
		AdvanceStage();
	}

	if (!Stages.IsValidIndex(CurrentStage)) return;

	const FNPCStage& Stage = Stages[CurrentStage];

	if (IsValid(Stage.StandPoint))
	{
		SetActorLocationAndRotation(Stage.StandPoint->GetActorLocation(), Stage.StandPoint->GetActorRotation());
	}

}

UEldenDialogueData* AEldenNPC::PickDialogue() const
{
	if (!Stages.IsValidIndex(CurrentStage)) return nullptr;
	return Stages[CurrentStage].Dialogue;
}

void AEldenNPC::AdvanceStage()
{
	if (Stages.IsValidIndex(CurrentStage + 1)) CurrentStage++;
	KillCount = 0;

}

void AEldenNPC::StartDialogue(AEldenCharacter* Player)
{
	CurrentDialogue = PickDialogue();
	if (!CurrentDialogue) return;

	bTalking = true;
	CurrentLineIndex = 0;
	CurrentPlayer = Player;

	Player->GetCharacterMovement()->StopMovementImmediately();
	Player->SetState(ECharacterState::Interacting);
	if (AEldenPlayerController* PC = GetCurrentPC())
	{
		PC->ShowDialogue(CurrentDialogue->GetSpeakerName(), CurrentDialogue->GetLine(CurrentLineIndex));
	}
}

void AEldenNPC::AdvanceDialogue()
{
	if (CurrentDialogue->IsLastLine(CurrentLineIndex))
	{
		if (Stages.IsValidIndex(CurrentStage))
		{
			const FNPCStage& Stage = Stages[CurrentStage];
			if (Stage.Advance == ENPCStageAdvance::AfterTalk)
			{
				if (Stage.RewardRunes > 0 && CurrentPlayer.IsValid() && CurrentPlayer->StatComponent)
				{
					CurrentPlayer->StatComponent->AddRunes(Stage.RewardRunes);
				}
				AdvanceStage();
			}
		}
		EndDialogue();
		return;
	}
	++CurrentLineIndex;

	if (AEldenPlayerController* PC = GetCurrentPC())
	{
		PC->ShowDialogue(CurrentDialogue->GetSpeakerName(), CurrentDialogue->GetLine(CurrentLineIndex));
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
	CurrentDialogue = nullptr;

}

AEldenPlayerController* AEldenNPC::GetCurrentPC() const
{
	if (!CurrentPlayer.IsValid()) return nullptr;
	return CurrentPlayer->GetEldenController();
}

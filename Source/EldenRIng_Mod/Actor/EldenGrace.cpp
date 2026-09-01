

#include "EldenRing_Mod/Actor/EldenGrace.h"
#include "Blueprint/UserWidget.h"
#include "GameFrameWork/PlayerController.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"


AEldenGrace::AEldenGrace()
{
 	
	PrimaryActorTick.bCanEverTick = true;
	// 컴포넌트 생성 및 계층 구조
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractSphere"));
	RootComponent = InteractionSphere;

	GraceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	GraceMesh->SetupAttachment(RootComponent);

	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEldenGrace::OnOverlapBegin);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AEldenGrace::OnOverlapEnd);
}


void AEldenGrace::BeginPlay()
{
	Super::BeginPlay();
	
}



void AEldenGrace::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEldenCharacter* Player = Cast<AEldenCharacter>(OtherActor);
	if (Player)
	{
		Player->CurrentInteractableTarget = this;
	}
}


void AEldenGrace::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AEldenCharacter* Player = Cast<AEldenCharacter>(OtherActor);
	if (Player)
	{
		Player->CurrentInteractableTarget = nullptr;
	}
}

void AEldenGrace::Interact(AEldenCharacter* Player)
{
	if (!Player) return;
	if (LevelUpWidgetClass)
	{
		UUserWidget* LevelUpWidget = CreateWidget<UUserWidget>(GetWorld(), LevelUpWidgetClass);
		if (LevelUpWidget)
		{
			LevelUpWidget->AddToViewport();

			if (APlayerController* PC = Cast < APlayerController>(Player->GetController()))
			{
				PC->bShowMouseCursor = true;

				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(LevelUpWidget->TakeWidget());
				PC->SetInputMode(InputMode);
			}
		}

		Player->GetCharacterMovement()->StopMovementImmediately();
		Player->SetState(ECharacterState::Interacting);
	}

	if (!Player->InventoryComponent) return;
	Player->InventoryComponent->RefillPotions();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Grace Found"));
	}
	UE_LOG(LogTemp, Log, TEXT("Grace Interact Called!"));

}


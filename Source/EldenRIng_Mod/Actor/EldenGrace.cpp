

#include "EldenRing_Mod/Actor/EldenGrace.h"
#include "Blueprint/UserWidget.h"
#include "GameFrameWork/PlayerController.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "EldenRing_Mod/EldenGameMode.h"
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

	RespawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RespawnPoint"));
	RespawnPoint->SetupAttachment(RootComponent);
	RespawnPoint->SetRelativeLocation(FVector(150.0f, 0.f, 90.f));
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

FTransform AEldenGrace::GetRespawnTransform() const
{
	return RespawnPoint->GetComponentTransform();
}

void AEldenGrace::Interact(AEldenCharacter* Player)
{
	if (!Player) return;

	if (LevelUpWidgetClass)
	{
		Player->OpenLevelUpMenu(LevelUpWidgetClass);
	}

	if (AEldenGameMode* GM = GetWorld()->GetAuthGameMode<AEldenGameMode>())
	{
		GM->HandleGraceRest(this, Player);
	}

}


#include "EldenRing_Mod/Actor/EldenAreaTrigger.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Controller/EldenPlayerController.h"
#include "Components/BoxComponent.h"

AEldenAreaTrigger::AEldenAreaTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AreaTrigger"));
	RootComponent = TriggerBox;
	TriggerBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	TriggerBox->SetBoxExtent(FVector(500.f, 500.f, 250.f));

}

void AEldenAreaTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AEldenAreaTrigger::OnOverlapBegin);
	TArray<AActor*> Overlapping;
	TriggerBox->GetOverlappingActors(Overlapping, AEldenCharacter::StaticClass());
	UE_LOG(LogTemp, Warning, TEXT("[AreaTrigger] BeginPlay 시점에 겹친 플레이어 수: %d"), Overlapping.Num());
}

void AEldenAreaTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("[AreaTrigger] OnOverlapBegin: %s"), *GetNameSafe(OtherActor));
	if (AEldenCharacter* Player = Cast<AEldenCharacter>(OtherActor))
	{
		AEldenPlayerController* PC = Player->GetEldenController();
		if (!PC) return;
		PC->ShowAreaName(AreaName, ShowDelay);
	}
}

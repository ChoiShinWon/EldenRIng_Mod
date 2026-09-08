

#include "EldenRing_Mod/Actor/Bloodstain.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"

ABloodstain::ABloodstain()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractSphere"));
	RootComponent = InteractionSphere;

	// BloodstainMesh 생성 후 RootComponent에 SetupAttachment
	BloodstainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BloodstainMesh"));
	BloodstainMesh->SetupAttachment(RootComponent);

	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABloodstain::OnOverlapBegin);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &ABloodstain::OnOverlapEnd);
}


void ABloodstain::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABloodstain::InitBloodstain(int32 Amount)
{
	StoredRunes = Amount;
}

void ABloodstain::Interact(AEldenCharacter* Player)
{

	// 널가드
	if (!Player || !Player->StatComponent) return;

	// 룬 되돌려주기
	Player->StatComponent->AddRunes(StoredRunes);

	// VFX 재생
	if (PickupVFX) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupVFX, GetActorLocation());


	// 플레이어가 이 액터를 가리키던 포인터 끊기
	Player->CurrentInteractableTarget = nullptr;

	
	Destroy();
}



void ABloodstain::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (AEldenCharacter* Player = Cast<AEldenCharacter>(OtherActor))
	{
		Player->CurrentInteractableTarget = this;
	}
}

void ABloodstain::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AEldenCharacter* Player = Cast<AEldenCharacter>(OtherActor))
	{
		// 오버랩 끝나면 가리키던 포인터 끊기
		Player->CurrentInteractableTarget = nullptr;
	}
}

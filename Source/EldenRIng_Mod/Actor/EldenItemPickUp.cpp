#include "EldenRing_Mod/Actor/EldenItemPickUp.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

AEldenItemPickUp::AEldenItemPickUp()
{
	PrimaryActorTick.bCanEverTick = false;
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	RootComponent = InteractionSphere;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(RootComponent);

	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEldenItemPickUp::OnOverlapBegin);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AEldenItemPickUp::OnOverlapEnd);

}

void AEldenItemPickUp::Interact(AEldenCharacter* Player)
{
	if (!Player || !Player->InventoryComponent || !ItemDef) return;

	Player->InventoryComponent->AddItem(ItemDef, PickAmount);

	Player->SetInteractableTarget(nullptr);

	Destroy();
}

FText AEldenItemPickUp::GetInteractionPrompt() const
{
	if (!ItemDef) return FText::FromString(TEXT("???"));

	FText InteractPrompt = FText::Format(FText::FromString(TEXT("{0} 줍기")), ItemDef->DisplayName);
	return InteractPrompt;
}


void AEldenItemPickUp::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (AEldenCharacter* Player = Cast<AEldenCharacter>(OtherActor))
	{
		Player->SetInteractableTarget(this);
	}
}

void AEldenItemPickUp::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AEldenCharacter* Player = Cast<AEldenCharacter>(OtherActor))
	{
		Player->SetInteractableTarget(nullptr);
	}
}

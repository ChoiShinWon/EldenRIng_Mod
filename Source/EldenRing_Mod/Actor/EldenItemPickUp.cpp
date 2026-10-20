#include "EldenRing_Mod/Actor/EldenItemPickUp.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Controller/EldenPlayerController.h"
#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "EldenRing_Mod/Component/EldenInteractionComponent.h"
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

	// 인벤토리에 몇 개 더 들어가는지
	int32 Addable = Player->InventoryComponent->GetRemainingCapacity(ItemDef);

	// 이번에 실제로 넣을 개수
	int32 ToAdd = FMath::Min(PickAmount, Addable);

	// 하나도 못 넣으면 거부
	if (ToAdd <= 0) return;

	bool bIsNew = !Player->InventoryComponent->HasItem(ItemDef);
	Player->InventoryComponent->AddItem(ItemDef, ToAdd);

	AEldenPlayerController* PC = Player->GetEldenController();

	if (PC)
	{
		PC->ShowItemAcquired(ItemDef, ToAdd, bIsNew);
	}

	// 남은 게 있는 부분 획득이면 여기서 끝
	PickAmount -= ToAdd;
	if (PickAmount > 0) return;
	// 곧 Destroy될 액터이므로, 상호작용 대상 참조를 먼저 비워서 댕글링 방지
	Player->InteractionComponent->SetInteractableTarget(nullptr);

	Destroy();
}

FText AEldenItemPickUp::GetInteractionPrompt() const
{
	return FText::FromString(TEXT("줍기"));

}


void AEldenItemPickUp::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (AEldenCharacter* Player = Cast<AEldenCharacter>(OtherActor))
	{
		Player->InteractionComponent->SetInteractableTarget(this);
	}
}

void AEldenItemPickUp::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AEldenCharacter* Player = Cast<AEldenCharacter>(OtherActor))
	{
		Player->InteractionComponent->ClearInteractableTarget(this);
	}
}

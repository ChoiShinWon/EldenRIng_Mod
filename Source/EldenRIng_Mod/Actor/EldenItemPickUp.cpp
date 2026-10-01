#include "EldenRing_Mod/Actor/EldenItemPickUp.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
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

	Player->InventoryComponent->AddItem(ItemDef, PickAmount);

	// 곧 Destroy될 액터이므로, 상호작용 대상 참조를 먼저 비워서 댕글링 방지
	Player->InteractionComponent->SetInteractableTarget(nullptr);

	Destroy();
}

FText AEldenItemPickUp::GetInteractionPrompt() const
{
	if (!ItemDef) return FText::FromString(TEXT("???"));

	// 이름을 따로 저장 안하고 ItemDef->DisplayName 재사용 (데이터 에셋에서 설정)
	FText InteractPrompt = FText::Format(FText::FromString(TEXT("{0} 줍기")), ItemDef->DisplayName);
	return InteractPrompt;
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
		Player->InteractionComponent->SetInteractableTarget(nullptr);
	}
}

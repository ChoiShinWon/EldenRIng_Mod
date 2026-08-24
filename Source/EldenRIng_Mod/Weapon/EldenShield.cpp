

#include "EldenRing_Mod/Weapon/EldenShield.h"
#include "Components/StaticMeshComponent.h"
#include "EldenRing_Mod/Component/EldenHitboxComponent.h"


AEldenShield::AEldenShield()
{
	PrimaryActorTick.bCanEverTick = false;

	// ≈ı∏Ì«— ∫Û æ¿ ƒƒ∆˜≥Õ∆Æ∏¶ √÷ªÛ¿ß ª—∏Æ∑Œ ªÔ¿Ω
	USceneComponent* DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	RootComponent = DefaultRoot;
	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetupAttachment(RootComponent);
	ShieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ShieldHitbox = CreateDefaultSubobject<UEldenHitboxComponent>(TEXT("ShieldHitbox"));
	ShieldHitbox->SetupAttachment(ShieldMesh);

}

void AEldenShield::BeginPlay()
{
	Super::BeginPlay();
	
}


void AEldenShield::EnableShieldBlock()
{
	if (ShieldHitbox)
	{
		ShieldHitbox->EnableHitbox();
	}
}

void AEldenShield::DisableShieldBlock()
{
	if (ShieldHitbox)
	{
		ShieldHitbox->DisableHitbox();
	}
}

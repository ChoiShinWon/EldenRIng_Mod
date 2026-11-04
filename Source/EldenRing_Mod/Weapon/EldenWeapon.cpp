// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/Weapon/EldenWeapon.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "EldenRing_Mod/Component/EldenHitboxComponent.h"
#include "Components/StaticMeshComponent.h"



// Sets default values
AEldenWeapon::AEldenWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
	RootComponent = WeaponRoot;
	
	// 외형 컴포넌트 생성
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));

	WeaponMesh->SetupAttachment(WeaponRoot);
	// 일단 장착 전에는 칼날이 캐릭터를 밀어내지 않도록 충돌을 끔
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// 박스 컴포넌트 생성 및 무기에 부착
	WeaponHitbox = CreateDefaultSubobject<UEldenHitboxComponent>(TEXT("WeaponHitBox"));
	WeaponHitbox->SetupAttachment(WeaponMesh);

}



void AEldenWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}



void AEldenWeapon::EnableWeaponCollision()
{
	float FinalDamage = BaseDamage;

	if (AEldenCharacter* MyOwner = Cast<AEldenCharacter>(GetOwner()))
	{
		if (UEldenStatComponent* OwnerStats = MyOwner->StatComponent)
		{
			FinalDamage += OwnerStats->AttackPower;
		}
	}

	if (WeaponHitbox)
	{
		WeaponHitbox->DamageAmount = FinalDamage;
		WeaponHitbox->EnableHitbox();
	}
	



}


void AEldenWeapon::DisableWeaponCollision()
{
	if (WeaponHitbox)
	{
		WeaponHitbox->DisableHitbox();
	}
}

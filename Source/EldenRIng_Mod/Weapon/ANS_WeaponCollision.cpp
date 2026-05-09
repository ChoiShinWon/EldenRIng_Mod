// Fill out your copyright notice in the Description page of Project Settings.

#include "EldenRing_Mod/Weapon/ANS_WeaponCollision.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Weapon/EldenWeapon.h"
#include "Components/BoxComponent.h"

void UANS_WeaponCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	// 1. 주인을 찾습니다.
	if (AEldenCharacter* Character = Cast<AEldenCharacter>(MeshComp->GetOwner()))
	{
		if (AEldenWeapon* Weapon = Character->GetEquippedWeapon())
		{
			Weapon->ClearHitActors();
		
			Weapon->GetWeaponBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		
	}
	
}

void UANS_WeaponCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	// 주인 찾기
	if (AEldenCharacter* Character = Cast<AEldenCharacter>(MeshComp->GetOwner()))
	{
		if (Character->GetEquippedWeapon())
		{
			Character->GetEquippedWeapon()->GetWeaponBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}


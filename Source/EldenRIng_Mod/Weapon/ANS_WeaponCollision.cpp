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
		if(AEldenWeapon * Weapon = Character->GetEquippedWeapon())
		{
			UE_LOG(LogTemp, Warning, TEXT("[1] 노티파이 실행: 무기 콜리전 ON 요청"));
			Weapon->EnableWeaponCollision();
		}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[1-에러] 캐릭터가 들고 있는 무기(EquippedWeapon)가 nullptr입니다!"));
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
		if (AEldenWeapon* Weapon = Character->GetEquippedWeapon())
		{
			Weapon->DisableWeaponCollision();
		}
	}
}


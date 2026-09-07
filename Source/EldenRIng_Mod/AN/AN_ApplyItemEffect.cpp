// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/AN/AN_ApplyItemEffect.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"

void UAN_ApplyItemEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner()) return;

	if (AEldenCharacter* Player = Cast<AEldenCharacter>(MeshComp->GetOwner()))
	{
		Player->ApplyItemEffect();
	}

	
}

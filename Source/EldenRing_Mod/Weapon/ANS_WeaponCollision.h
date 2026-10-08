// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_WeaponCollision.generated.h"

/**
 * 
 */
UCLASS()
class ELDENRING_MOD_API UANS_WeaponCollision : public UAnimNotifyState
{
	GENERATED_BODY()
	
public: 
	// 노티파이 구간이 시작될 때 실행 (박스 켜기)
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation, 
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	// 노티파이 구간이 끝날 때 실행 (박스 끄기)
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
		const FAnimNotifyEventReference& EventReference) override;
};
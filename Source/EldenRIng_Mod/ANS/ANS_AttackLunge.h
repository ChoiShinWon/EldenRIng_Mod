// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_AttackLunge.generated.h"

/**
 * 
 */
UCLASS()
class ELDENRING_MOD_API UANS_AttackLunge : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	float LungeSpeed = 0.0f;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};

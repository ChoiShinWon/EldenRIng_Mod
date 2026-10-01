// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "AnimModifier_FlattenPelvis.generated.h"

UCLASS()
class ELDENRING_MOD_API UAnimModifier_FlattenPelvis : public UAnimationModifier
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FName PelvisBoneName = "pelvis";

	virtual void OnApply_Implementation(UAnimSequence* AnimationSequence) override;
};

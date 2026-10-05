// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_EnemyAttack.generated.h"


UENUM(BlueprintType)
enum class EEnemyAttackType : uint8
{
	RightHand UMETA(DisplayName = "Right"),
	LeftHand UMETA(DisplayName = "Left"),
	BothHands UMETA(DisplayName = "Both")
};

UCLASS()
class ELDENRING_MOD_API UANS_EnemyAttack : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	EEnemyAttackType AttackType;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};

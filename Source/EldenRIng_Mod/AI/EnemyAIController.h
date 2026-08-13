// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class ELDENRING_MOD_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
public:

	void SetAggroTarget(AActor* Target);

	void ClearAggroTarget();

};

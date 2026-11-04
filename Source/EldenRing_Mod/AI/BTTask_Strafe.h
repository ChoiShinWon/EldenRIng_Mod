// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Strafe.generated.h"

/**
 * 
 */
UCLASS()
class ELDENRING_MOD_API UBTTask_Strafe : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Strafe();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 얼마나 옆으로 이동할지	결정하는 변수
	UPROPERTY(EditAnywhere, Category = "AI")
	float StrafeDistance = 300.0f; 
	
};

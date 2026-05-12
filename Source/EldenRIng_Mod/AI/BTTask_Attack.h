// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class ELDENRING_MOD_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_Attack();

protected:
	// Behavior Tree가 이 Task를 실행할 때 호출되는 함수.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

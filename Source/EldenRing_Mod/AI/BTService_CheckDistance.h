// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckDistance.generated.h"

/**
 * 
 */
UCLASS()
class ELDENRING_MOD_API UBTService_CheckDistance : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_CheckDistance();

protected:
	// 틱마다 실행할 함수
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// 에디터에서 설정할 최대 어그로 해제 거리 변수
	UPROPERTY(EditAnywhere, Category = "AI")
	float MaxAggroDistance = 1500.0f;
};


#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_FindWanderLocation.generated.h"

UCLASS()
class ELDENRING_MOD_API UBTTask_FindWanderLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindWanderLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 배회 반경 (Home 기준)
	UPROPERTY(EditAnywhere, Category = "Wander")
	float WanderRadius = 600.0f;

	// 결과를 쓸 블랙보드 vector 키, 에디터 드롭다운에서 고름
	UPROPERTY(EditAnywhere, Category = "Wander")
	FBlackboardKeySelector WanderLocationKey;
};

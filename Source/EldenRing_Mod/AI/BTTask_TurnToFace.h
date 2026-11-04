
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_TurnToFace.generated.h"

struct FTurnToFaceMemory
{
	float StartYaw; // 시작 시점의 액터 Yaw
	float DeltaYaw; // 돌아야 할 총 각도
	float ElapsedTime; // 시작 후 경과 시간
	float Duration; // 회전에 걸릴 시간 = 몽타주 길이
	UAnimMontage* PlayingMontage; // 이 적이 지금 재생 중인 턴 몽타주
};

UCLASS()
class ELDENRING_MOD_API UBTTask_TurnToFace : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_TurnToFace();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual uint16 GetInstanceMemorySize() const override;

	// 돌아볼 대상. Vector 키와 Object키를 둘 다 받는다
	// 배회에서는 WanderLocation, 전투에서는 TargetActor를 에디터에서 고름
	UPROPERTY(EditAnywhere, Category = "Turn")
	FBlackboardKeySelector TargetKey;

	// 이 각도 미만이면 몽타주 없이 통과
	UPROPERTY(EditAnywhere, Category = "Turn", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float MinTurnAngle = 45.0f;

	
};

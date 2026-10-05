#include "EldenRing_Mod/AI/BTTask_FindWanderLocation.h"
#include "EldenRing_Mod/Character/EldenEnemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"


UBTTask_FindWanderLocation::UBTTask_FindWanderLocation()
{
	NodeName = TEXT("Find Wander Location");

	WanderLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindWanderLocation, WanderLocationKey));

}

EBTNodeResult::Type UBTTask_FindWanderLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!AIC || !BB) return EBTNodeResult::Failed;

	AEldenEnemy* Enemy = Cast<AEldenEnemy>(AIC->GetPawn());
	if (!Enemy) return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) return EBTNodeResult::Failed;

	FNavLocation Result;
	bool bFound = NavSys->GetRandomReachablePointInRadius(Enemy->HomeLocation, WanderRadius, Result);
	if (!bFound) return EBTNodeResult::Failed;

	BB->SetValueAsVector(WanderLocationKey.SelectedKeyName, Result.Location);

	return EBTNodeResult::Succeeded;
}

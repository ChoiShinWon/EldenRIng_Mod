#include "EldenRing_Mod/AI/BTTask_Attack.h"
#include "AIController.h"
#include "EldenRing_Mod/Character/EldenEnemy.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		AEldenEnemy* Enemy = Cast<AEldenEnemy>(AIController->GetPawn());
		if (Enemy)
		{
			Enemy->PlayAttackMontage();
			if (Enemy->GetIsAttacking())
			{
				return EBTNodeResult::InProgress;
			}
			else
			{
				return EBTNodeResult::Failed;
			}
		}
	}
	return EBTNodeResult::Failed;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	AEldenEnemy* Enemy = Cast<AEldenEnemy>(AIC->GetPawn());
	if (!Enemy)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}


	if (!Enemy->GetIsAttacking())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}


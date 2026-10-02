
#include "EldenRing_Mod/AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EldenRing_Mod/Character/EldenEnemy.h"

const FName AEnemyAIController::BBKey_Target(TEXT("TargetActor"));
const FName AEnemyAIController::BBKey_Aggroed(TEXT("bAggroed"));
const FName AEnemyAIController::BBKey_Stunned(TEXT("bStunned"));


void AEnemyAIController::SetAggroTarget(AActor* Target)
{
	if (UBlackboardComponent* BBComp = GetBlackboardComponent())
	{
		BBComp->SetValueAsObject(BBKey_Target, Target);
	}
}

void AEnemyAIController::ClearAggroTarget()
{
	// Gameplay 우선순위로 건 포커스는 이름 그대로 해제하지 않으면 영원히 남는다.
	ClearFocus(EAIFocusPriority::Gameplay);
	if (UBlackboardComponent* BBComp = GetBlackboardComponent())
	{
		BBComp->ClearValue(BBKey_Target);
	}
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AEldenEnemy* Enemy = Cast<AEldenEnemy>(InPawn);

	if (Enemy && Enemy->EnemyBT)
	{
		RunBehaviorTree(Enemy->EnemyBT);
	}
}

void AEnemyAIController::SetAlerted(bool bAlerted)
{
	if (UBlackboardComponent* BBComp = GetBlackboardComponent())
	{
		BBComp->SetValueAsBool(BBKey_Aggroed, bAlerted);
	}
}


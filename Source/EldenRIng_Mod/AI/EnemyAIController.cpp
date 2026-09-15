// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EldenRing_Mod/Character/EldenEnemy.h"

void AEnemyAIController::SetAggroTarget(AActor* Target)
{
	if (UBlackboardComponent* BBComp = GetBlackboardComponent())
	{
		BBComp->SetValueAsObject(FName("TargetActor"), Target);
	}
}

void AEnemyAIController::ClearAggroTarget()
{
	if (UBlackboardComponent* BBComp = GetBlackboardComponent())
	{
		BBComp->ClearValue(FName("TargetActor"));
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


// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

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


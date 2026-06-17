// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/AI/BTTask_Attack.h"
#include "AIController.h"
#include "EldenRing_Mod/Character/EldenEnemy.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack");
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
			// 몽타주가 끝날 때까지 기다리지 않고 즉시 성공을 반환하면
			// 몬스터가 공격 모션 도중에 딴짓을 할 수 있음.
			// 나중에 델리게이트로 보완하기
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}


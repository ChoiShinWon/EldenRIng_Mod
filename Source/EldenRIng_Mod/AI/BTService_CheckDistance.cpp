// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/AI/BTService_CheckDistance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EldenRing_Mod/Character/EldenEnemy.h"
#include "AIController.h"

UBTService_CheckDistance::UBTService_CheckDistance()
{
	NodeName = TEXT("Check Aggro Distance");
}

void UBTService_CheckDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	
	if (BlackBoardComp)
	{
		AActor* TargetActor = Cast<AActor>(BlackBoardComp->GetValueAsObject(FName("TargetActor")));

		if (ControlledPawn && TargetActor)
		{
			// 캐릭터와 몬스터 사이 거리
			float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), TargetActor->GetActorLocation());
			// 둘의 거리가 최대 어그로 수치보다 멀어진다면
			if (Distance > MaxAggroDistance)
			{
				// ControlledPawn EldenEnemy 캐스팅
				AEldenEnemy* Enemy = Cast<AEldenEnemy>(ControlledPawn);

				if (Enemy)
				{
					// 어그로 취소
					Enemy->ResetAggro();
				}
		
			}
		}
		
	}

}

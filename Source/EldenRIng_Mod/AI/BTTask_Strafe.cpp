// Fill out your copyright notice in the Description page of Project Settings.


#include "Eldenring_Mod/AI/BTTask_Strafe.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"	
#include "NavigationSystem.h"

UBTTask_Strafe::UBTTask_Strafe()
{
	NodeName = TEXT("Strafe Around Player");
}

EBTNodeResult::Type UBTTask_Strafe::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	APawn* Pawn = AIC->GetPawn();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(FName("TargetActor")));
	if (!TargetActor || !Pawn) return EBTNodeResult::Failed;

	// 플레이어와 몬스터의 위치를 가져옴
	FVector PlayerLoc = TargetActor->GetActorLocation();
	FVector MonsterLoc = Pawn->GetActorLocation();

	// 플레이어를 향하는 방향 벡터 계산
	FVector DirectionToMonster = (MonsterLoc - PlayerLoc).GetSafeNormal();

	float SideStepDir = FMath::RandBool() ? 1.0f : -1.0f; // 왼쪽 또는 오른쪽으로 이동할 방향 결정
	// 플레이어를 중심으로 옆으로 이동할 벡터 계산
	FVector SideVector = FVector::CrossProduct(FVector::UpVector, DirectionToMonster) * SideStepDir; 

	// 최종 목표 위치 계산
	FVector GoalLocation = MonsterLoc + (SideVector * StrafeDistance);

	AIC->SetFocus(TargetActor);

	// 네비게이션 시스템을 사용하여 목표 위치로 이동
	AIC->MoveToLocation(GoalLocation, 50.0f, true);
	
	return EBTNodeResult::Succeeded;
}

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
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AIC || !BB) return EBTNodeResult::Failed;
	APawn* Pawn = AIC->GetPawn(); // 이 Task를 실행 중인 AI(몬스터) 자신
	
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(FName("TargetActor")));
	if (!TargetActor || !Pawn) return EBTNodeResult::Failed;

	// 플레이어와 몬스터의 위치를 가져옴
	FVector PlayerLoc = TargetActor->GetActorLocation();
	FVector MonsterLoc = Pawn->GetActorLocation();

	// 몬스터 위치 - 플레이어 위치
	FVector PlayerToMonsterDir = (MonsterLoc - PlayerLoc).GetSafeNormal();

	float SideStepDir = FMath::RandBool() ? 1.0f : -1.0f; // 랜덤으로 왼쪽 또는 오른쪽으로 이동할 방향 결정

	// UpVector(0, 0, 1)와 DirerctionToMonster의 외적은
	// 플레이어-몬스터를 잇는 축에 대해 수평면에서 정확히 90도 꺾인 벡터를 만들어냄
	// 즉 몬스터가 플레이어를 중심으로 원을 그리며 도는 방향(접선 벡터)을 구하는 표준적인 방법
	// SideStepDir(+1/-1)을 곱해서 시계/반시계 중 랜덤하게 하나를 선택
	FVector SideVector = FVector::CrossProduct(FVector::UpVector, PlayerToMonsterDir) * SideStepDir; 

	// 최종 목표 위치 계산
	FVector GoalLocation = MonsterLoc + (SideVector * StrafeDistance);

	AIC->SetFocus(TargetActor);
	// 네비게이션 시스템을 사용하여 목표 위치로 이동
	AIC->MoveToLocation(GoalLocation, 50.0f, true);
	
	return EBTNodeResult::Succeeded;
}

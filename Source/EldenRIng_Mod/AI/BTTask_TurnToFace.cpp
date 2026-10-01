#include "EldenRing_Mod/AI/BTTask_TurnToFace.h"
#include "AIController.h"
#include "EldenRing_Mod/Character/EldenEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"


UBTTask_TurnToFace::UBTTask_TurnToFace()
{
	NodeName = TEXT("Turn To Face");
	bNotifyTick = true;
	TargetKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_TurnToFace, TargetKey));
	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_TurnToFace, TargetKey), AActor::StaticClass());


}

EBTNodeResult::Type UBTTask_TurnToFace::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	AEldenEnemy* Enemy = Cast<AEldenEnemy>(AIC->GetPawn());
	if (!Enemy) return EBTNodeResult::Failed;

	// 어디를 볼 것인가: 블랙보드에서 목표 위치 읽기
	// TargetKey가 Vector 키 (WanderLocation)면 그 값을,
	// Object 키(TargetActor)면 그 액터의 위치를 돌려준다.
	FVector TargetLoc;
	if(!BB->GetLocationFromEntry(TargetKey.SelectedKeyName, TargetLoc)) return EBTNodeResult::Failed;

	// 얼마나 돌아야 하는가: 각도 계산
	// 내 위치에서 목표를 향하는 방향 벡터
	FVector ToTarget = TargetLoc - Enemy->GetActorLocation();
	// 높이 차이는 무시.  목표가 위 아래여도 몸은 수평으로만 돈다
	ToTarget.Z = 0.f;
	// 목표가 내 바로 위 (수평 거리 0)면 방향이 정의되지 않으므로 돌 필요 없음
	if (ToTarget.IsNearlyZero()) return EBTNodeResult::Succeeded;

	// 지금 내가 보는 방향(Yaw)  나중에 TickTask가 여기서부터 보간하므로 변수로 보관
	const float CurrentYaw = Enemy->GetActorRotation().Yaw;

	// 목표 방향의 Yaw - 내 Yaw = 돌아야 할 각도
	// NoramalizeAxis: 결과를 -180~+180으로 감는다. 안하면 350 -> 10 를 -340로 계산해서 반대로 한 바퀴 돈다.
	// 양수 = 오른쪽, 음수 = 왼쪽
	const float Delta = FRotator::NormalizeAxis(ToTarget.Rotation().Yaw - CurrentYaw);
	// 이미 거의 그쪽을 보고 있으면 애니 없이 통과 (이 정도는 CMC의 회전 속도로 자연스럽게 처리됨).
	if (FMath::Abs(Delta) < MinTurnAngle) return EBTNodeResult::Succeeded;

	// ===== 어느 쪽 몽타주 목록을 쓸 것인가 =====
	// 부호로 좌우 결정. const& 로 받아서 배열 복사를 피한다 (읽기만 하니까).
	const TArray<FTurnMontageEntry>& Entries = (Delta > 0.f) ? Enemy->TurnRightEntries : Enemy->TurnLeftEntries;
	// 턴 애니가 없는 몬스터면 배열이 비어있음
	if (Entries.IsEmpty()) return EBTNodeResult::Succeeded;

	// 그중 필요한 각도에 가장 가까운 몽타주 구하기
	// 아직 아무것도 못 골랐다는 뜻의 nullptr
	const FTurnMontageEntry* Best = nullptr;
	// 가장 작은 차이를 찾는 루프의 시작값은 가능한 가장 큰 값이어야 첫 엔트리 채택
	float BestDiff = FLT_MAX;

	for (const FTurnMontageEntry& Entry : Entries)
	{
		// BP에서 몽타주를 비워 둔 칸은 건너뜀
		if (!Entry.Montage) continue;
		// 이 엔트리가 담당하는 각도와 필요한 각도 (|Delta|)의 차이
		float Diff = FMath::Abs(Entry.Angle - FMath::Abs(Delta));
		// 더 가까운 후보가 나오면 숫자와 그 엔트리를 함께 갱신
		if (Diff < BestDiff)
		{
			BestDiff = Diff;
			Best = &Entry;
		}
	}
	// 유효한 몽타주가 하나도 없으면 통과
	if (!Best) return EBTNodeResult::Succeeded;

	// 이 메시의 애님 인스턴스(애님 BP의 실행 객체). 몽타주는 여기서 재생한다.
	UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return EBTNodeResult::Failed;

	// 재생에 성공하면 몽타주 길이(초), 실패하면 0 반환. 이 길이가 곧 "회전에 걸릴 시간".
	const float Length = AnimInstance->Montage_Play(Best->Montage);
	// 재생 실패면 여기서 끝낸다 (return 없이 내려가면 Duration=0이 되어 TickTask가 0으로 나눈다).
	if (Length <= 0.f) return EBTNodeResult::Succeeded;

	// 이 적의 회전 진행 상태를 저장
	FTurnToFaceMemory* Mem = CastInstanceNodeMemory<FTurnToFaceMemory>(NodeMemory);
	Mem->StartYaw = CurrentYaw;
	Mem->DeltaYaw = Delta;
	Mem->ElapsedTime = 0.f;
	Mem->Duration = Length;
	Mem->PlayingMontage = Best->Montage;

	// 여기서 Succeeded를 반환하면 BT가 바로 다음 노드(MoveTo)로 가버려서 회전할 시간이 없다.
	// InProgress를 반환하면 TickTask가 호출되고, 거기서 FinishLatentTask를 불러야 다음으로 넘어간다.
	return EBTNodeResult::InProgress;
}

void UBTTask_TurnToFace::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FTurnToFaceMemory* Mem = CastInstanceNodeMemory<FTurnToFaceMemory>(NodeMemory);
	AAIController* AIC = OwnerComp.GetAIOwner();
	APawn* Pawn = AIC ? AIC->GetPawn() : nullptr;
	if (!Pawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	Mem->ElapsedTime += DeltaSeconds;
	const float Alpha = FMath::Clamp(Mem->ElapsedTime / Mem->Duration, 0.f, 1.f);

	// 이번 프레임의 Yaw
	const float NewYaw = Mem->StartYaw + Mem->DeltaYaw * Alpha;
	Pawn->SetActorRotation(FRotator(0.f, NewYaw, 0.f));

	if (Alpha >= 1.f) FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

EBTNodeResult::Type UBTTask_TurnToFace::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 내가 재생한 몽타주가 무엇이었는지 꺼내기
	FTurnToFaceMemory* Mem = CastInstanceNodeMemory<FTurnToFaceMemory>(NodeMemory);

	// 몽타주를 재생한 AnimInstance 찾기
	AAIController* AIC = OwnerComp.GetAIOwner();
	ACharacter* Character = AIC ? Cast<ACharacter>(AIC->GetPawn()) : nullptr;
	UAnimInstance* AnimInstance = Character ? Character->GetMesh()->GetAnimInstance() : nullptr;

	// 내 몽타주만 멈추기
	if (AnimInstance && Mem->PlayingMontage)
	{
		AnimInstance->Montage_Stop(0.2f, Mem->PlayingMontage);
	}

	return EBTNodeResult::Aborted;
}

uint16 UBTTask_TurnToFace::GetInstanceMemorySize() const
{
	return sizeof(FTurnToFaceMemory);
}

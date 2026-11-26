#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyCombatComponent.generated.h"

class UAnimMontage;

// ─────────────────────────────────────────────────────────
// 공격 "상황 라벨". 거리 값이 아니다.
//
// 컴포넌트는 플레이어와의 거리를 재지 않는다.
// 거리 판단은 BT(데코레이터)가 하고, BT의 Attack 태스크가
// "지금은 이 상황이다"라는 라벨을 넘긴다.
// 컴포넌트는 그 라벨이 붙은 패턴만 후보로 보고, 가중치로 하나를 고른다.
//
//   예) Grux
//     Patterns:  [기본 공격→슬램 : Melee]   [Stampede : Mid]
//     BT 근접 분기                    → Attack(Melee) → 기본 공격
//     BT 중거리 분기 (거리 600~1200)  → Attack(Mid)   → Stampede
//
// 라벨을 나누는 이유: 거리 숫자를 BT에만 두기 위해서다.
// (컴포넌트에도 거리 범위를 두면 BT와 숫자가 이중으로 생겨 어긋난다)
// 새 상황이 필요하면(예: 후퇴 중 공격) 값을 추가하고 BT 분기를 하나 더 만든다.
// ─────────────────────────────────────────────────────────

UENUM(BlueprintType)
enum class EEnemyAttackGroup : uint8
{
	Melee UMETA(DisplayName = "Melee"), // 근접 공격 (BT 근접 분기에서 호출)
	Mid UMETA(DisplayName = "Mid") // 중거리 공격: 돌진 등 (BT 중거리 분기에서 호출)
};

// 공격 패턴 한 칸. 몬스터별 BP의 Patterns 배열에 채운다.
USTRUCT(BlueprintType)
struct FEnemyAttackPattern
{
	GENERATED_BODY()

	// 가장 먼저 재생할 몽타주, 비어 있으면 후보에서 제외된다.
	UPROPERTY(EditAnywhere)
	UAnimMontage* Montage = nullptr;

	// 첫 몽타주 뒤에 이어서 재생할 몽타주. 비워 두며 단일 몽타주 패턴.
	UPROPERTY(EditAnywhere)
	UAnimMontage* FollowUpMontage = nullptr;

	// 후속 몽타주로 이어질 확률 (0~1). 패턴 시작 시 한 번만 굴린다. 후속 몽타주가 없으면 무시
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FollowUpChance = 1.0f;

	// 이 패턴의 상황 라벨. BT가 넘긴 라벨과 같은 패턴만 후보가 된다.
	UPROPERTY(EditAnywhere)
	EEnemyAttackGroup Group = EEnemyAttackGroup::Melee;

	// 같은 Group 안에서 뽑힐 상대적 확률 (가중치)
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float Weight = 1.0f;
};

// 패턴이 끝났을 때(정상 종료, 중단 모두) 컴포넌트가 Enemy에게 보내는 신호
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyPatternEnd);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDENRING_MOD_API UEnemyCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnemyCombatComponent();

	// Group에 맞는 패턴을 골라 재생한다. 성공하면 true
	// 후보 없음, 재생 실패, 이미 진행 중이면 false
	bool StartAttack(EEnemyAttackGroup Group);

	// 진행 중인 패턴 무효화(후속 예약 취소)
	void CancelPattern();

	// 패턴 종료 신호(델리게이트)
	UPROPERTY(BlueprintAssignable)
	FOnEnemyPatternEnd OnPatternEnd;

protected:
	// 이 몬스터의 공격 패턴 배열
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TArray<FEnemyAttackPattern> Patterns;

private:
	// 이번 패턴에서 이어서 재생할 후속 몽타주. 패턴 시작시 FollowUpChance로 한 번 굴려서 정한다.
	UPROPERTY()
	UAnimMontage* PendingFollowUp = nullptr;

	// 패턴 진행 중 여부
	bool bPatternActive = false;

	// 몽타주 콜백
	// 연결
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	// 종료
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	bool PlayPatternMontage(UAnimMontage* Montage, bool bHasFollowUp);

	// 선택 헬퍼
	const FEnemyAttackPattern* ChoosePattern(EEnemyAttackGroup Group) const;
};

#include "EldenRing_Mod/Component/EnemyCombatComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

// 소우 캐릭터의 AnimInstance를 얻는 이 파일 전용 헬퍼
// 캐싱하지 않는 이유: 공격 시작/몽타주 콜백에서만 불리고 매 프레임 호출이 아니라서.
static UAnimInstance* GetOwnerAnim(const AActor* Owner)
{
	const ACharacter* Char = Cast<ACharacter>(Owner);
	if (!Char) return nullptr;
	USkeletalMeshComponent* Mesh = Char->GetMesh();
	if (!Mesh) return nullptr;
	return Mesh->GetAnimInstance();
}

UEnemyCombatComponent::UEnemyCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

// Group이 같은 후보들 중에서 가중치대로 랜덤으로 하나 고른다. (거리 판단은 BT가 이미 끝낸 상태)
// 알고리즘 = "숫자 직선 위에 주사위 던지기"
// 후보 가중치가 A=1, B=3, C=1 이면 직선 [0~5]를 1:3:1 칸으로 나누고,
// Roll(0~5)이 떨어진 칸의 후보를 고른다. 그래서 합계(Total)를 먼저 알아야 하고 순회가 두 번이다.
const FEnemyAttackPattern* UEnemyCombatComponent::ChoosePattern(EEnemyAttackGroup Group) const
{
	// Group 일치, Montage 유효, Weight > 0 (가중치 0은 절대 안뽑힘)
	auto IsCandidate = [Group](const FEnemyAttackPattern& P) {return P.Group == Group && P.Montage && P.Weight > 0.f; };

	// [1차 순회] 주사위 범위를 정하기 위해 후보 가중치의 합계만 구한다.
	float Total = 0.f;
	for (const FEnemyAttackPattern& P : Patterns) // 값으로 복사하면 &P가 복사본 주소가 되어서 댕글링
	{
		if (IsCandidate(P)) Total += P.Weight;
	}
	if (Total <= 0.f) return nullptr; // 이 Group의 후보가 없음 -> 호출한 쪽 (StartAttack)이 false 처리

	// 주사위: 0 ~ Total 사이의 한 점
	float Roll = FMath::FRandRange(0.f, Total);
	// 부동소수 오차로 루프에서 못 골랐을 때를 위한 안정장치 (마지막 후보를 대신 반환)
	const FEnemyAttackPattern* Last = nullptr;

	// [2차 순회] 앞에서부터 가중치를 빼 나가다가 0이하가 되는 첫 후보가 주사위가 떨어진 칸이다.
	for (const FEnemyAttackPattern& P : Patterns)
	{
		if( !(IsCandidate(P)) ) continue;
		Last = &P;
		Roll -= P.Weight;
		if (Roll <= 0.f) return &P;
	}
	return Last;
}

// 몽타주 1개를 재생하고 "끝났을 때 불릴 콜백"을 건다. 재생 실패 시 false.
// 콜백은 몽타주 하나에 정확히 하나만 건다:
// 뒤에 후속이 있으면 (bHasFollowUp) BlendingOut 콜백 -> 블렌드 아웃이 시작될 때 후속으로 이어 간다.
// 후속이 없으면 End 콜백 -> 완전히 끝났을 때 패턴 종료를 알린다.
bool UEnemyCombatComponent::PlayPatternMontage(UAnimMontage* Montage, bool bHasFollowUp)
{
	UAnimInstance* Anim = GetOwnerAnim(GetOwner());
	if (!Anim) return false;

	const float Len = Anim->Montage_Play(Montage);
	if (Len <= 0) return false;

	// BindUObject는 약참조 바인딩이라 컴포넌트가 먼저 사라져도 크래시하지 않는다
	if (bHasFollowUp)
	{
		FOnMontageBlendingOutStarted D;
		D.BindUObject(this, &UEnemyCombatComponent::OnMontageBlendingOut);
		Anim->Montage_SetBlendingOutDelegate(D, Montage);
	}
	else
	{
		FOnMontageEnded D;
		D.BindUObject(this, &UEnemyCombatComponent::OnMontageEnded);
		Anim->Montage_SetEndDelegate(D, Montage);
	}
	return true;
}

// 외부 (Enemy)가 부르는 진입점. BT가 넘긴 Group에 맞는 패턴을 골라 재생한다.
// 반환값 true일 때만 Enemy가 상태를 Attacking으로 바꾼다
bool UEnemyCombatComponent::StartAttack(EEnemyAttackGroup Group)
{
	if (bPatternActive) return false;
	const FEnemyAttackPattern* P = ChoosePattern(Group);
	if (!P) return false;

	// 후속 연결 여부
	// FollowUpChance = 1.0이면 FRand() < 1.0 이 항상 참, 0이면 항상 거짓
	PendingFollowUp = (P->FollowUpMontage && FMath::FRand() < P->FollowUpChance) ? P->FollowUpMontage : nullptr;
	bPatternActive = true;

	// 재생에 실패하면 방금 켠 상태를 원상복구한다
	if (!PlayPatternMontage(P->Montage, PendingFollowUp != nullptr))
	{
		bPatternActive = false;
		PendingFollowUp = nullptr;
		return false;
	}
	return true;
}

// 후속 몽타주가 있는 패턴의 앞 몽타주가 블렌드 아웃을 시작할 때 불린다.
// 여기서 후속을 재생하면 앞 몽타주가 사라지는 동안 겹쳐서 부드럽게 이어진다
void UEnemyCombatComponent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bPatternActive) return;

	// 정상적으로 끝났고 예약된 후속이 있으면 이어서 재생
	if (!bInterrupted && PendingFollowUp)
	{
		UAnimMontage* Next = PendingFollowUp;
		PendingFollowUp = nullptr; // 먼저 비운다 (두 번 재생 방지)
		if (PlayPatternMontage(Next, false)) return;
	}

	// 여기까지 왔다면 중단됐거나 후속이 없거나 후속 재생이 실패한 것
	OnMontageEnded(Montage, bInterrupted);
}

// 패턴의 마지막 몽타주가 끝났을 때 불린다. 패턴을 마무리하고 Enemy에게 알린다
void UEnemyCombatComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bPatternActive) return;
	bPatternActive = false;
	PendingFollowUp = nullptr;
	OnPatternEnd.Broadcast();
}

// 진행중인 패턴 무효화.
// 스턴/사망/리셋 때 Enemy가 몽타주를 멈추기 전에 호출
void UEnemyCombatComponent::CancelPattern()
{
	bPatternActive = false;
	PendingFollowUp = nullptr;
}

#pragma once

#include "CoreMinimal.h"
#include "EldenStatModifier.generated.h"

// 스탯 보정에 쓰는 공용 타입 모음.
// 무엇을 / 어떻게 / 얼마나 바꾸는가를 표현한다. 예) 최대 체력 / 고정으로 / +20
// 탈리스만에 한정된 타입이 아니다. 같은 타입으로 다른 효과도 표현할 수 있다.


// 보정하는 스탯은 레벨업 스탯이 아니라 계산된 파생 스탯이다.
// 레벨업 스탯을 올리면 레벨, 레벨업 UI까지 같이 바뀌기 때문.
UENUM(BlueprintType)
enum class EStatModifierTarget : uint8
{
	None UMETA(DisplayName = "없음"),
	MaxHealth UMETA(DisplayName = "최대 체력"),
	MaxStamina UMETA(DisplayName = "최대 스태미너"),
	MaxMana UMETA(DisplayName = "최대 마나"),
	AttackPower UMETA(DisplayName = "공격력"),
	StaminaRegenRate UMETA(DisplayName = "스태미너 회복 속도"),
	DamageTaken UMETA(DisplayName = "받는 피해") // Percent기준 +면 받는 피해 증가, -면 감소
};


// 값을 더하는 방식. 합산 단계에서 두 종류를 따로 모으기 위해 나눠 둔다.
UENUM(BlueprintType)
enum class EModifierOp : uint8
{
	// 고정값을 더함 (예: 최대 체력 +20)
	Flat UMETA(DisplayName = "고정"),
	// 비율로 증감 (예: 공격력 +10%)
	Percent UMETA(DisplayName = "퍼센트")
};

// 스탯을 바꾸는 효과 한 건
// 탈리스만 DA가 이걸 배열로 들고 있다.
// 효과는 여러 개 넣을 수 있다. 좋은 효과와 나쁜 효과를 같이 넣으면 "대가가 있는 물건"이 된다.
// 예) "공격력 +10%" + "받는 피해 +15%"
// 값이 마이너스(-)면 줄어든다는 뜻이다. 예) 공격력 -10%면 약해짐.
// 멤버는 DA에서 읽기만 하는 데이터 묶음이라 public으로 둠.
USTRUCT(BlueprintType)
struct FEldenStatModifier
{
	GENERATED_BODY()

	// 어떤 파생 스탯을 건드리는가
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modifier")
	EStatModifierTarget Target = EStatModifierTarget::None;

	// 값을 더하는 방식 (고정/퍼센트)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modifier")
	EModifierOp Op = EModifierOp::Flat;

	// 효과의 크기, 마이너스 값도 정상(줄이는 효과)이라서 ClampMin(최솟값 제한)은 일부러 걸지 않았다
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modifier", meta = (ToolTip = "Flat: 더할 값 (20 = +20). Percent: 비율 (0.1 = +10%). 음수 = 감소"))
	float Value = 0.f;

};

// 합산 결과 한 칸: 같은 스탯에 걸린 효과들의 합계
// Flat과 Percent를 한 float로 합치면 "+20과 +10%"가 섞여 버려서 따로 모은다
// StatComponent가 키 하나로 조회하면 둘을 한 번에 받는다.
// Percent는 비율의 합 (0.1 + 0.2 = 0.3 -> 30%)
struct FEldenStatBonus
{
	float Flat = 0.f;
	float Percent = 0.f;
};

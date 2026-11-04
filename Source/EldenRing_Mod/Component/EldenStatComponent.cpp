

#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "EldenRing_Mod/StatUtils.h"
#include "Engine/World.h" // 타이머 매니저(GetWorld())를 사용하기 위해 필수
#include "TimerManager.h"

UEldenStatComponent::UEldenStatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;


}


void UEldenStatComponent::BeginPlay()
{
	Super::BeginPlay();

	RecalculateDerivedStats();

	// 게임 시작 시 체력과 마나, 스태미너를 꽉 채워줌.
	CurrentHealth = MaxHealth;
	CurrentMana = MaxMana;
	CurrentStamina = MaxStamina;
	
}


void UEldenStatComponent::LoseAllRunes()
{
	CurrentRunes = 0;
	OnRunesChanged.Broadcast(CurrentRunes);
}


void UEldenStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 1. 스태미너 자동 회복 로직 (캐릭터에서 이사 옴)
	if (bCanRegen)
	{
		if (CurrentStamina < MaxStamina)
		{
			// FStatUtils를 사용하여 안전하게 더하고 최대치를 넘지 않도록 제한합니다.
			FStatUtils::UpdateStat(CurrentStamina, MaxStamina, (StaminaRegenRate * DeltaTime));
			OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
		}
		else
		{
			SetComponentTickEnabled(false);
		}
	}
}

void UEldenStatComponent::SetEquipmentBonus(const TMap<EStatModifierTarget, FEldenStatBonus>& InBonus)
{
	EquipmentBonus = InBonus;
	RecalculateDerivedStats();
}

// 최종값 = 기준값 * (1 + 퍼센트) + 고정값
// 고정값은 퍼센트의 영향을 받지 않는다.
// 결과가 음수면 0으로 막는다. 퍼센트가 -100%보다 많이 깎여도 최대치가 음수가 되지 않게 하려는 것.
float UEldenStatComponent::CalcWithBonus(float BaseValue, EStatModifierTarget Target) const
{
	// 맵에 Target 칸이 없으면 0, 0짜리 기본값이 나온다. 그래서 보너스가 없을 땐 기준값이 그대로 반환된다.
	const FEldenStatBonus Bonus = EquipmentBonus.FindRef(Target);
	return FMath::Max(0.f, BaseValue * (1.f + Bonus.Percent) + Bonus.Flat);
}




void UEldenStatComponent::ApplyDamage(float DamageAmount)
{

	// 받는 피해 보정. Percent는 배율. Flat은 한 방당 가감.
	// DamageAmount > 0 일 때만 적용: 가드로 0이 된 피격에 Flat 페널티가 붙어 체력이 닳는 것을 막으려는 것.
	if (DamageAmount > 0)
	{
		DamageAmount = CalcWithBonus(DamageAmount, EStatModifierTarget::DamageTaken);
	}


	// 데미지 적용 로직 (FStatUtils 활용)
	// 데미지이므로 음수(-)를 붙여서 깎아줍니다.
	FStatUtils::UpdateStat(CurrentHealth, MaxHealth, -DamageAmount);

	// 체력이 변했으니 OnHealthChanged 방송
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	// 체력이 0 이하가 되면 방송(Broadcast)을 때립니다!
	if (CurrentHealth <= 0.0f)
	{
		OnZeroHealth.Broadcast();
	}
}

bool UEldenStatComponent::LevelUpStat(EEldenStatType StatToLevelUp)
{
	if (CurrentRunes < RequiredRunes)
	{
		return false;
	}

	CurrentRunes -= RequiredRunes;
	OnRunesChanged.Broadcast(CurrentRunes);
	Level += 1;
	RequiredRunes += 500;

	switch (StatToLevelUp)
	{
	case EEldenStatType::Vigor:
		Vigor += 1;
		break;

	case EEldenStatType::Mind:
		Mind += 1;
		break;
	case EEldenStatType::Endurance:
		Endurance += 1;
		break;

	case EEldenStatType::Strength:
		Strength += 1;
		break;
	}

	RecalculateDerivedStats();

	return true;
}

void UEldenStatComponent::RecalculateDerivedStats()
{
	float OldMaxHealth = MaxHealth;
	float OldMana = MaxMana;

	// 레벨 스탯(Vigor 등)으로 기준값을 만들고 장비 보너스를 반영해 파생 스탯(MaxHealth 등)을 재계산
	MaxHealth = CalcWithBonus(100.0f + (Vigor * 25.0f), EStatModifierTarget::MaxHealth);
	MaxMana = CalcWithBonus(100.0f + (Mind * 25.0f), EStatModifierTarget::MaxMana);
	MaxStamina = CalcWithBonus(100.0f + (Endurance * 2.0f), EStatModifierTarget::MaxStamina);
	AttackPower = CalcWithBonus(3 + (Strength * 3.0f), EStatModifierTarget::AttackPower);

	// 회복 속도는 StaminaRegenRate가 아니라 BaseStaminaRegenRate에서 계산한다.
	// 자기 자신에 곱하면 재계산할 때마다 보너스가 쌓여서 (20 -> 22 -> 24.2) 안 된다.
	StaminaRegenRate = CalcWithBonus(BaseStaminaRegenRate, EStatModifierTarget::StaminaRegenRate);

	SetComponentTickEnabled(true);

	// MaxHealth가 늘어난 만큼 CurrentHealth도 그대로 밀어올림
	// 단순히 CurrentHealth를 새 MaxHealth로 클램프/리셋 해버리면
	// 체력 80/100이던 캐릭터가 레벨업으로 Max가 150이 되었을 때 70을 잃은 채로 시작하는게 아니라
	// 그냥 80/150(=70 잃은 상태 유지)이 되어야 하는데
	// 잃었던 체력의 절대량을 그대로 보존하기 위해 계산
	// 최대치가 줄어드는 경우도 같은 식이 처리한다.
	// 장착하면 +N, 해제하면 -N이라 장착/해제를 반복해도 체력을 공짜로 얻을 수 없다. 마나도 같은 식.
	CurrentHealth += (MaxHealth - OldMaxHealth);
	CurrentMana += (MaxMana - OldMana);

	// 최대치가 줄었을 때 현재 값이 범위를 벗어나지 않게 자른다.
	// 체력 하한이 1인 이유: 위 델타가 음수가 되어 0 이하가 돼도 ApplyDamage를 안 거쳐서 OnZeroHealth가 안 뜬다.
	//   그러면 체력 0인데 안 죽는 상태가 되므로, 장비를 풀다가 죽는 일이 없게 최소 1로 둔다.
	// 스태미너는 상한만 자른다. 하한 0은 ConsumeStamina의 Clamp가 이미 막고 있다.
	CurrentHealth = FMath::Clamp(CurrentHealth, 1.f, MaxHealth);
	CurrentMana = FMath::Clamp(CurrentMana, 0.f, MaxMana);
	CurrentStamina = FMath::Min(CurrentStamina, MaxStamina);

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnManaChanged.Broadcast(CurrentMana, MaxMana);
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

void UEldenStatComponent::ConsumeStamina(float Amount)
{
	// 실제 스태미너 깎기
	CurrentStamina = FMath::Clamp(CurrentStamina - Amount, 0.0f, MaxStamina);

	// 스태미너가 변했으니 OnStaminaChanged 방송
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);

	// 회복 중단
	bCanRegen = false;

	// 타이머 재설정
	// 만약 1.5초 뒤에 회복하기로 예약했는데, 그 사이에 다시 구르면 기존 예약은 취소해야 하므로 Clear 하고 들어감.
	GetWorld()->GetTimerManager().ClearTimer(RegenDelayTimerHandle);
	// Clear후 다시 Timer 세팅, 시간이 StaminaRegen Delay후엔 Regen 다시 진행
	GetWorld()->GetTimerManager().SetTimer(RegenDelayTimerHandle, this,
		&UEldenStatComponent::ResetRegen, StaminaRegenDelay, false);

	SetComponentTickEnabled(true);
}

void UEldenStatComponent::ResetRegen()
{
	bCanRegen = true;
}

void UEldenStatComponent::AddRunes(int32 Amount)
{
	CurrentRunes += Amount;
	OnRunesChanged.Broadcast(CurrentRunes);
}


bool UEldenStatComponent::IsHealthFull() const
{
	return CurrentHealth >= MaxHealth;
}

void UEldenStatComponent::Heal(float HealAmount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UEldenStatComponent::FullRestore()
{
	CurrentHealth = MaxHealth;
	CurrentMana = MaxMana;
	CurrentStamina = MaxStamina;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnManaChanged.Broadcast(CurrentMana, MaxMana);
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

bool UEldenStatComponent::HasEnoughMana(float Amount) const
{
	return CurrentMana >= Amount;
}

void UEldenStatComponent::ConsumeMana(float Amount)
{
	CurrentMana = FMath::Clamp(CurrentMana - Amount, 0.0f, MaxMana);
	OnManaChanged.Broadcast(CurrentMana, MaxMana);
}

void UEldenStatComponent::RestoreMana(float Amount)
{
	CurrentMana = FMath::Clamp(CurrentMana + Amount, 0.0f, MaxMana);
	OnManaChanged.Broadcast(CurrentMana, MaxMana);
}

bool UEldenStatComponent::IsManaFull() const
{
	return CurrentMana >= MaxMana;
}

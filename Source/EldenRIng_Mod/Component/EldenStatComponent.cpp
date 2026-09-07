// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "EldenRing_Mod/StatUtils.h"
#include "Engine/World.h" // 타이머 매니저(GetWorld())를 사용하기 위해 필수
#include "TimerManager.h"

// Sets default values for this component's properties
UEldenStatComponent::UEldenStatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;


}


// Called when the game starts
void UEldenStatComponent::BeginPlay()
{
	Super::BeginPlay();

	RecalculateDerivedStats();

	// 게임 시작 시 체력과 스태미너를 꽉 채워줌.
	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;
	
}


// Called every frame
void UEldenStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 1. 스태미너 자동 회복 로직 (캐릭터에서 이사 옴)
	if (bCanRegen && CurrentStamina < MaxStamina)
	{
		// FStatUtils를 사용하여 안전하게 더하고 최대치를 넘지 않도록 제한합니다.
		FStatUtils::UpdateStat(CurrentStamina, MaxStamina, (StaminaRegenRate * DeltaTime));
		OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
	}
}




void UEldenStatComponent::ApplyDamage(float DamageAmount)
{
	// 2. 데미지 적용 로직 (FStatUtils 활용)
	// 데미지이므로 음수(-)를 붙여서 깎아줍니다.
	FStatUtils::UpdateStat(CurrentHealth, MaxHealth, -DamageAmount);

	// 체력이 변했으니 OnHealthChanged 방송
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	// 3. 체력이 0 이하가 되면 방송(Broadcast)을 때립니다!
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
	MaxHealth = 100.0f + (Vigor * 25.0f);
	MaxStamina = 100.0f + (Endurance * 2.0f);
	AttackPower = 3 + (Strength * 3.0f);
	
	CurrentHealth += (MaxHealth - OldMaxHealth);
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
	GetWorld()->GetTimerManager().SetTimer(RegenDelayTimerHandle, this, &UEldenStatComponent::ResetRegen, StaminaRegenDelay, false);

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
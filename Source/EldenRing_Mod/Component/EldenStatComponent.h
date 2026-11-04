#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EldenRing_Mod/EldenStatModifier.h"
#include "EldenStatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnZeroHealthDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChangeDelegate, float, CurrentValue, float, MaxValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRunesChangeDelegate, int32, NewRunes);

UENUM(BlueprintType)
enum class EEldenStatType : uint8
{
	Vigor       UMETA(DisplayName = "생명력"),
	Mind        UMETA(DisplayName = "정신력"),
	Endurance   UMETA(DisplayName = "지구력"),
	Strength    UMETA(DisplayName = "근력")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDENRING_MOD_API UEldenStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEldenStatComponent();

protected:
	
	virtual void BeginPlay() override;

public:
	/*=============================================================================
	 * 기초 스탯 & 재화 시스템
	 *=============================================================================*/
	// 전체 레벨, 생명력, 지구력, 근력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Vigor = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Mind = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Endurance = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Strength = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 CurrentRunes = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 RequiredRunes = 500;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRunesChangeDelegate OnRunesChanged;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool LevelUpStat(EEldenStatType StatToLevelUp);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void RecalculateDerivedStats();

	/*=============================================================================
	 * 체력 시스템 (Health)
	 *=============================================================================*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth = 100.0f;

	void ApplyDamage(float DamageAmount);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnZeroHealthDelegate OnZeroHealth;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStatChangeDelegate OnHealthChanged;

	// 체력이 꽉 찼는지 확인하는 함수
	bool IsHealthFull() const;

	void Heal(float HealAmount);

	void FullRestore();

	/*=============================================================================
	* 정신력 시스템 (Mana)
	*=============================================================================*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mana")
	float MaxMana = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mana")
	float CurrentMana = 100.0f;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStatChangeDelegate OnManaChanged;

	UFUNCTION(BlueprintPure, Category = "Mana")
	bool HasEnoughMana(float Amount) const;

	UFUNCTION(BlueprintCallable, Category = "Mana")
	void ConsumeMana(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Mana")
	void RestoreMana(float Amount);

	// 마나가 꽉 찼는지 확인하는 함수
	bool IsManaFull() const;


	/*=============================================================================
	 * 스태미너 시스템 (Stamina)
	 *=============================================================================*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float CurrentStamina = 100.0f;

	// 효과값. 기준값에 장비 보너스를 반영한 계산 결과라서 에디터에서 못 고치게 했다. Tick이 이 값을 읽는다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
	float StaminaRegenRate = 20.f;

	// 기준값. 에디터에서 직접 정하는 값이다. 재계산할 때마다 이 값에서 효과값을 새로 만든다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float BaseStaminaRegenRate = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRegenDelay = 1.5f;

	bool bCanRegen = true;

	FTimerHandle RegenDelayTimerHandle;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStatChangeDelegate OnStaminaChanged;


	// 스태미너 소모 함수
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void ConsumeStamina(float Amount);

	void ResetRegen();

	// 룬을 더해주는 전용 함수
	UFUNCTION(BlueprintCallable, Category = "Rune")
	void AddRunes(int32 Amount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float AttackPower = 3;

	// 사망시 룬을 모두 잃는 함수
	void LoseAllRunes();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// HUD에서 값을 읽어갈 수 있도록 Getter 추가
	FORCEINLINE float GetCurrentStamina() const { return CurrentStamina; }
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }

	// HUD 에서 값을 읽어갈 수 있도록 Getter 추가
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	// 마나용 게터함수
	FORCEINLINE float GetCurrentMana() const { return CurrentMana; }
	FORCEINLINE float GetMaxMana() const { return MaxMana; }

	// Equipment가 호출할 입구
	void SetEquipmentBonus(const TMap<EStatModifierTarget, FEldenStatBonus>& InBonus);

private:

	// 합산 결과 보관. 레벨업 때 Recalculate가 다시 읽으니까 저장해둬야 함.
	TMap<EStatModifierTarget, FEldenStatBonus> EquipmentBonus;

	// 맵에서 Target 칸을 찾아 보너스를 반영한 최종값을 돌려줌. 멤버를 안 바꾸니 const
	float CalcWithBonus(float BaseValue, EStatModifierTarget Target) const;
};

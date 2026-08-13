// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EldenStatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnZeroHealthDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChangeDelegate, float, CurrentValue, float, MaxValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRunesChangeDelegate, int32, NewRunes);

UENUM(BlueprintType)
enum class EEldenStatType : uint8
{
	Vigor       UMETA(DisplayName = "생명력"),
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

	/*=============================================================================
	 * 스태미너 시스템 (Stamina)
	 *=============================================================================*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float CurrentStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRegenRate = 20.f;

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
public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// HUD에서 값을 읽어갈 수 있도록 Getter 추가
	FORCEINLINE float GetCurrentStamina() const { return CurrentStamina; }
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }

	// HUD 에서 값을 읽어갈 수 있도록 Getter 추가
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
};

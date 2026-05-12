
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EldenEnemy.generated.h"

class UWidgetComponent;

UCLASS()
class ELDENRING_MOD_API AEldenEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEldenEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// 몬스터의 최대 체력과 현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* HitReactMontage;
	
	// 죽음 상태 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* TargetMarkWidget;
public:	

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, class AActor* DamageCauser) override;

	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE bool GetIsDead() const { return bIsDead; }

	void ShowTargetMark(bool bShow);
};

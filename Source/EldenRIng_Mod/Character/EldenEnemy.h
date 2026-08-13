
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/PawnSensingComponent.h"
#include "EldenRing_Mod/Interface/ITargetable.h"
#include "EldenEnemy.generated.h"

class UWidgetComponent;
class UEldenHitboxComponent;
class UParticleSystem;

UCLASS()
class ELDENRING_MOD_API AEldenEnemy : public ACharacter, public IITargetable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEldenEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AEnemyAIController* EnemyController;
	


	// 몬스터의 시야(눈) 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "AI")
	class UPawnSensingComponent* PawnSensingComp;

	// 플레이어를 감지했는지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bHasAggro = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bHasRoared = false;

	// 몬스터의 최대 체력과 현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentHealth;
	
	// 몬스터의 애니메이션 몽타주들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* AggroMontage;

	

	// 현재 타겟으로 삼고 있는 플레이어 폰
	UPROPERTY()
	class APawn* CombatTarget;
	
	// 몬스터가 죽었을 때 호출되는 함수
	virtual void Die();

	// 죽음 애니메이션이 끝났을 때 호출되는 함수 (델리게이트로 연결)
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 죽음 상태 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDead = false;

	// 타겟 마크 위젯 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* TargetMarkWidget;

	/*=============================================================================
	 * 공격 타격 판정 (Hitbox)
	 *=============================================================================*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UEldenHitboxComponent* RightHandHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UEldenHitboxComponent* LeftHandHitbox;

	/*=============================================================================
	 * 룬 보상 시스템
	 *=============================================================================*/
	// 몬스터 잡았을 때 플레이어에게 줄 룬의 양
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	int32 RuneReward = 100;
	
public:	
	// AI가 몬스터에 빙의할 때 엔진이 자동으로 호출해 주는 함수
	virtual void PossessedBy(AController* NewController) override;

	// 시야에 플레이어가 들어왔을 때 호출되는 함수
	UFUNCTION()
	void OnSeePlayer(APawn* Pawn);	

	// Damage를 입었을 때 호출되는 함수 (데미지 처리 로직 포함)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, class AActor* DamageCauser) override;

	// 현재 체력, 최대 체력, 죽음 여부를 반환하는 함수들
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE bool GetIsDead() const { return bIsDead; }

	// 어그로 종료 함수
	void ResetAggro();

	// 어그로 애니메이션이 끝났을 때 호출되는 함수 
	UFUNCTION()
	void OnAggroMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 공격 애니메이션을 재생하는 함수
	void PlayAttackMontage();

	// 공격 애니메이션이 끝났을 때 호출되는 함수 (델리게이트	로 연결)
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 공격 중인지 여부 (콤보 시스템 구현 시 활용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking = false;

	virtual bool IsTargetable() const override;
	virtual void ShowTargetMark(bool bShow) override;

	void EnableRightAttackCollision();
	void DisableRightAttackCollision();
	void EnableLeftAttackCollision();
	void DisableLeftAttackCollision();
};

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimMontage.h"
#include "EldenCombatComponent.generated.h"

class AEldenEnemy;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ELDENRING_MOD_API UEldenCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEldenCombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	class UAnimMontage* ParryMontage;

	// 현재 콤보 번호
	int32 ComboCount = 0;
	// 다음 콤보가 예약되었는지 확인하는 플래그
	bool bComboQueued = false;


	void ExecuteAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void CheckComboQueue();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void ExecuteBlock();
	void EndBlock();

	void ExecuteParry();

	// ExecuteParry랑 거의 동일한 구조
	void ExecuteWeaponSkill();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// AN_WeaponSkillHit이 임팩트 프레임에 호출할 함수
	void PerformSkillStrike();


	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Combat|Parry")
	float ParryFacingDot = 0.0f;


	UPROPERTY(EditAnywhere, Category = "Combat|Parry")
    class UParticleSystem* ParryVFX;

    UPROPERTY(EditAnywhere, Category = "Combat|Parry")
    class USoundBase* ParrySound;

	// 역경직을 관리할 타이머 핸들
	FTimerHandle HitStopTimerHandle;

	//bool TryParryHit();   // 성공 시 true

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Parry")
	bool bParryWindowActive = false;

	void SetParryWindowActive(bool bActive) { bParryWindowActive = bActive; }

	// 여기서 이 공격을 막을 수 있냐 질의
	bool TryDeflect(const FVector& HitLocation, AEldenEnemy* Attacker);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnParryMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
private:

	void ResetTimeDilation();

	// 소유주인 캐릭터의 애니메이션 인스턴스를 캐싱하기 위한 변수
	UPROPERTY()
	class UAnimInstance* CachedAnimInstance;

	// 플레이어 캐릭터 캐싱
	UPROPERTY()
	class AEldenCharacter* PlayerCharacter;

	// 포인터로 리턴해야 nullptr 가능
	const TArray<UAnimMontage*>* GetCurrentComboMontages() const;

};

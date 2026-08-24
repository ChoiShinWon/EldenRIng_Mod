#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimMontage.h"
#include "EldenCombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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


	// 콤보 공격용 몽타주 섹션이 3개로 나뉘어 있어야함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TArray<UAnimMontage*> ComboMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	class UAnimMontage* ParryMontage;

	// 현재 콤보 번호
	int32 ComboCount = 0;
	// 다음 콤보가 예약되었는지 확인하는 플래그
	bool bComboQueued = false;

	//// 공격 중인지 판별하는 변수
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	//bool bIsAttacking;

	void ExecuteAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void CheckComboQueue();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void ExecuteBlock();
	void EndBlock();

	void ExecuteParry();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnParryMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
private:

	// 소유주인 캐릭터의 애니메이션 인스턴스를 캐싱하기 위한 변수
	UPROPERTY()
	class UAnimInstance* CachedAnimInstance;

	// 플레이어 캐릭터 캐싱
	UPROPERTY()
	class AEldenCharacter* PlayerCharacter;
};

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
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 콤보 공격용 몽타주 섹션이 3개로 나뉘어 있어야함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* ComboMontage;

	// 현재 콤보 번호
	int32 ComboCount = 0;
	// 다음 콤보가 예약되었는지 확인하는 플래그
	bool bComboQueued = false;

	// 공격 중인지 판별하는 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking;

	void ExecuteAttack();

	// 콤보 창 제어용 함수 (애님 노티파이에서 호출)
	UFUNCTION(BlueprintCallable)
	void SetComboWindow(bool bOpen);

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 콤보 시작 진행 횟수
	void ProcessCombo();
private:

	// 소유주인 캐릭터의 애니메이션 인스턴스를 캐싱하기 위한 변수
	UPROPERTY()
	class UAnimInstance* CachedAnimInstance;
};

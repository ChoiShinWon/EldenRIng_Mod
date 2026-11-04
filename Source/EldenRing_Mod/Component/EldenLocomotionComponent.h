
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EldenLocomotionComponent.generated.h"

class AEldenCharacter;
class UAnimMontage;
class UEldenStatComponent;
class UCharacterMovementComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDENRING_MOD_API UEldenLocomotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEldenLocomotionComponent();

	// 입력 노티파이 콤보 컴포넌트가 부르는 진입점들
	void Dodge();
	void StartSprint();
	void StopSprint();
	void StartAttackLunge(float Speed);
	void StopAttackLunge();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Character::Move가 입력마다 기록 -> 제자리 구르기 방향 계산용
	FVector2D LastMoveInput = FVector2D::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category ="Locomotion")
	UAnimMontage* RollMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Locomotion")
	float DodgeStaminaCost = 25.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Locomotion")
	float SprintStaminaCost = 10.0f;

protected:
	virtual void BeginPlay() override;


private:
	void OnRollMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// bIsSprinting || bIsLunging 이면 Tick ON, 아니면 OFF
	void UpdateTickState();

	// GC가 지우지 않게 / 이 컴포넌트는 Owner 수명과 같으니 캐시 OK
	UPROPERTY()
	AEldenCharacter* OwnerCharacter = nullptr;

	UPROPERTY()
	UEldenStatComponent* CachedStatComp = nullptr;


	UPROPERTY()
	UCharacterMovementComponent* CMC = nullptr;

	bool bIsSprinting = false;
	bool bIsLunging = false;
	float SavedWalkSpeedBeforeLunge = 0.0f;

};

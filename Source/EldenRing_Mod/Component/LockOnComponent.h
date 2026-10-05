#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EldenRing_Mod/Interface/ITargetable.h"
#include "LockOnComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ELDENRING_MOD_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	ULockOnComponent();
	// 인자 제거
	void ToggleLockOn();
	void UpdateLockOn(float DeltaTime);
	bool HasTarget() const { return CurrentTarget.IsValid(); }

protected:
	virtual void BeginPlay() override;

	// 읽기 전용 참조
	TWeakObjectPtr<class AActor> CurrentTarget;

	UPROPERTY()
	class AEldenCharacter* OwnerCharacter;

	void FindBestTarget();

	UPROPERTY(EditAnywhere, Category = "LockOn")
	float MaxLockOnDistance = 2500.0f;

	UPROPERTY(EditAnywhere, Category = "LockOn")
	float LockOnAcquireDistance = 1500.0f;

	IITargetable* GetTargetInterface() const;
};

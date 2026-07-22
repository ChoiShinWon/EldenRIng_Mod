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
	TWeakObjectPtr<class AEldenEnemy> CurrentTarget;

	void FindBestTarget();

	UPROPERTY(EditAnywhere, Category = "LockOn")
	float MaxLockOnDistance = 2000.0f;
};
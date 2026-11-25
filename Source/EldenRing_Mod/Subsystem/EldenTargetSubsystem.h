
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EldenTargetSubsystem.generated.h"

UCLASS()
class ELDENRING_MOD_API UEldenTargetSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void RegisterTarget(AActor* Target);

	void UnregisterTarget(AActor* Target);

	const TArray<TWeakObjectPtr<AActor>>& GetTargets() const;

private:
	TArray<TWeakObjectPtr<AActor>> Targets;
};

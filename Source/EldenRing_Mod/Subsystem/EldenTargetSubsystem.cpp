
#include "EldenRing_Mod/Subsystem/EldenTargetSubsystem.h"

void UEldenTargetSubsystem::RegisterTarget(AActor* Target)
{
	if (!Target) return;

	if (!Targets.Contains(Target))
	{
		Targets.Add(Target);
	}
}

void UEldenTargetSubsystem::UnregisterTarget(AActor* Target)
{
	if (!Target) return;

	int32 Removed = Targets.RemoveSingleSwap(Target);
	Targets.RemoveAll([](const TWeakObjectPtr<AActor>& P)
		{
			return !P.IsValid();
		});

}

const TArray<TWeakObjectPtr<AActor>>& UEldenTargetSubsystem::GetTargets() const
{
	return Targets;
}

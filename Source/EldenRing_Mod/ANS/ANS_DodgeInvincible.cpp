

#include "EldenRing_Mod/ANS/ANS_DodgeInvincible.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"

void UANS_DodgeInvincible::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (MeshComp && MeshComp->GetOwner())
	{
		if (AEldenCharacter* Player = Cast<AEldenCharacter>(MeshComp->GetOwner()))
		{
			Player->SetInvincible(true);
		}
	}
	
}

void UANS_DodgeInvincible::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		if (AEldenCharacter* Player = Cast<AEldenCharacter>(MeshComp->GetOwner()))
		{
			Player->SetInvincible(false);
		}
	}
	
}

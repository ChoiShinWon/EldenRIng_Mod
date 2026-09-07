#include "EldenRing_Mod/AN/AN_ParryCheck.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"

void UAN_ParryCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		// 애니메이션 주인이 플레이어라면 ParryCheck() 실행
		if (AEldenCharacter* Player = Cast<AEldenCharacter>(MeshComp->GetOwner()))
		{
			Player->ParryCheck();
		}
	}
}
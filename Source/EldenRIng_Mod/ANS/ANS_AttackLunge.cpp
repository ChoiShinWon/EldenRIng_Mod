

#include "EldenRing_Mod/ANS/ANS_AttackLunge.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"

void UANS_AttackLunge::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	AEldenCharacter* Player = Cast<AEldenCharacter>(MeshComp->GetOwner());

	if (!Player) return;
	UE_LOG(LogTemp, Warning, TEXT("[AttackLunge] NotifyBegin 호출됨! Speed=%f"), LungeSpeed);
	Player->StartAttackLunge(LungeSpeed);
}



void UANS_AttackLunge::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	AEldenCharacter* Player = Cast<AEldenCharacter>(MeshComp->GetOwner());
	if (!Player) return;
	Player->StopAttackLunge();
}

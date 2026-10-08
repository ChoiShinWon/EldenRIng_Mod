#include "EldenRing_Mod/AN/AN_WeaponSkillHit.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Component/EldenCombatComponent.h"


void UAN_WeaponSkillHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner()) return;

	AEldenCharacter* PlayerCharacter = Cast<AEldenCharacter>(MeshComp->GetOwner());

	if (!PlayerCharacter) return;
	if (!PlayerCharacter->CombatComponent) return;

	PlayerCharacter->CombatComponent->PerformSkillStrike();

}

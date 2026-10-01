

#include "EldenRing_Mod/ANS/ANS_AttackLunge.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"

void UANS_AttackLunge::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	// 이 노티파이는 어떤 스켈레탈 메시에든 붙을 수 있는 범용 컴포넌트이므로,
	// 실제로 우리 캐릭터에 붙었는지는 런타임에 Cast로 확인해야 함 (컴파일 타임엔 보장 안된다
	AEldenCharacter* Player = Cast<AEldenCharacter>(MeshComp->GetOwner());
	if (!Player) return;

	// 플레이어 함수에 에디터에서 지정한 LungeSpeed 값 전달
	Player->StartAttackLunge(LungeSpeed);
}



void UANS_AttackLunge::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	AEldenCharacter* Player = Cast<AEldenCharacter>(MeshComp->GetOwner());
	if (!Player) return;

	Player->StopAttackLunge();
}

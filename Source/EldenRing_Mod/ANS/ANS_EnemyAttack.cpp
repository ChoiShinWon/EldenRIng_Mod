

#include "EldenRing_Mod/ANS/ANS_EnemyAttack.h"
#include "EldenRing_Mod/Character/EldenEnemy.h"

void UANS_EnemyAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (MeshComp && MeshComp->GetOwner())
	{
		if (AEldenEnemy* EnemyRef = Cast<AEldenEnemy>(MeshComp->GetOwner()))
		{
			

			switch (AttackType)
			{
			case EEnemyAttackType::RightHand:
				EnemyRef->EnableRightAttackCollision();
				break;

			case EEnemyAttackType::LeftHand:
				EnemyRef->EnableLeftAttackCollision();
				break;

			case EEnemyAttackType::BothHands:
				EnemyRef->EnableLeftAttackCollision();
				EnemyRef->EnableRightAttackCollision();
				break;
			}
				
		}
	}
}

void UANS_EnemyAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		if (AEldenEnemy* EnemyRef = Cast<AEldenEnemy>(MeshComp->GetOwner()))
		{
			switch (AttackType)
			{
			case EEnemyAttackType::RightHand:
				EnemyRef->DisableRightAttackCollision();
				break;

			case EEnemyAttackType::LeftHand:
				EnemyRef->DisableLeftAttackCollision();
				break;

			case EEnemyAttackType::BothHands:
				EnemyRef->DisableLeftAttackCollision();
				EnemyRef->DisableRightAttackCollision();
				break;
			}
		}
	}
}

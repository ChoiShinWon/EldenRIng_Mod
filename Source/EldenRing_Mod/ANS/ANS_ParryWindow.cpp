// Fill out your copyright notice in the Description page of Project Settings.

#include "EldenRing_Mod/ANS/ANS_ParryWindow.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Component/EldenCombatComponent.h"

void UANS_ParryWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		// 이 애니메이션을 재생하는 주체가 몬스터라면? 패링 창 열기!
		if (AEldenCharacter* Player = Cast<AEldenCharacter>(MeshComp->GetOwner()))
		{
			if (Player->CombatComponent)
				Player->CombatComponent->SetParryWindowActive(true);
		}
	}
}

void UANS_ParryWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		// 애니메이션 지정 구간이 끝나면 패링 창 닫기!
		if (AEldenCharacter* Player = Cast<AEldenCharacter>(MeshComp->GetOwner()))
		{
			if (Player->CombatComponent)
				Player->CombatComponent->SetParryWindowActive(false);
		}
	}
}

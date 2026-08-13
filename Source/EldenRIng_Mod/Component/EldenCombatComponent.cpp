// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/Component/EldenCombatComponent.h"
#include "GameFramework/Character.h" 
#include "Containers/Array.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"


UEldenCombatComponent::UEldenCombatComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;

}



void UEldenCombatComponent::BeginPlay()
{
	Super::BeginPlay();

    PlayerCharacter = Cast<AEldenCharacter>(GetOwner());
    if (PlayerCharacter)
    {
        CachedAnimInstance = PlayerCharacter->GetMesh()->GetAnimInstance();
    }
	
}



void UEldenCombatComponent::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (bInterrupted)
    {
        if (PlayerCharacter && PlayerCharacter->GetState() == ECharacterState::Attacking)
        {
            return;
        }
    }
    bComboQueued = false;
    ComboCount = 0;
    
    if (PlayerCharacter)
    {
        PlayerCharacter->SetState(ECharacterState::Idle);
        if (PlayerCharacter->bDodgeQueued)
        {
            PlayerCharacter->bDodgeQueued = false;
            PlayerCharacter->Dodge();
        }
    }

}



void UEldenCombatComponent::ExecuteAttack()
{
    if (!PlayerCharacter || !CachedAnimInstance) return;

    if (PlayerCharacter->bDodgeQueued)
    {
        return;
    }

    if (ComboMontages.Num() == 0)
    {
        return;
    }
    

    if (PlayerCharacter && CachedAnimInstance)
    {
        if (PlayerCharacter->GetState() == ECharacterState::Idle)
        {
            PlayerCharacter->SetState(ECharacterState::Attacking);
            ComboCount = 1;
            bComboQueued = false;

            CachedAnimInstance->Montage_Play(ComboMontages[0], 1.0f);
            FOnMontageEnded EndDelegate;
            EndDelegate.BindUObject(this, &UEldenCombatComponent::OnAttackMontageEnded);
            CachedAnimInstance->Montage_SetEndDelegate(EndDelegate, ComboMontages[0]);
        }
        else if (PlayerCharacter->GetState() == ECharacterState::Attacking)
        {
            if (ComboCount < ComboMontages.Num())
            {
                bComboQueued = true;
            }
        }
    }
    
}

void UEldenCombatComponent::CheckComboQueue()
{
    if (!PlayerCharacter || !PlayerCharacter->GetMesh()) return;

    if (PlayerCharacter->bDodgeQueued) return;

    if (PlayerCharacter && CachedAnimInstance)
    {
        if (bComboQueued)
        {
            ComboCount++;
            bComboQueued = false;
            if (ComboCount <= ComboMontages.Num())
            {
                CachedAnimInstance->Montage_Play(ComboMontages[ComboCount - 1]);
                FOnMontageEnded EndDelegate;
                EndDelegate.BindUObject(this, &UEldenCombatComponent::OnAttackMontageEnded);
                CachedAnimInstance->Montage_SetEndDelegate(EndDelegate, ComboMontages[ComboCount - 1]);
            }

           
        }
    }
}

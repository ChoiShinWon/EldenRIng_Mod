// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/Component/EldenCombatComponent.h"
#include "GameFramework/Character.h" 
#include "Containers/Array.h"
#include "EldenRing_Mod/Weapon/EldenShield.h"
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
        if (PlayerCharacter->GetState() == ECharacterState::Attacking)
        {
            PlayerCharacter->SetState(ECharacterState::Idle);
        }
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

void UEldenCombatComponent::ExecuteBlock()
{
    if (!PlayerCharacter || !CachedAnimInstance) return;

    // 공격 중이거나 구르는 중이 아니면 가드 자세 진입 허용
    if (PlayerCharacter->GetState() == ECharacterState::Idle)
    {
        PlayerCharacter->SetState(ECharacterState::Blocking);

        // 필요하다면 방패 방어 히트박스 켜기
        if (AEldenShield* Shield = PlayerCharacter->GetEquippedShield())
        {
            Shield->EnableShieldBlock();
        }

        // TODO: 방패를 들고 서 있는 루프 애니메이션 또는 블렌드 포즈 적용
    }
}

void UEldenCombatComponent::EndBlock()
{
    if (!PlayerCharacter) return;

    // 가드 상태일 때만 해제 가능
    if (PlayerCharacter->GetState() == ECharacterState::Blocking)
    {
        PlayerCharacter->SetState(ECharacterState::Idle);

        if (AEldenShield* Shield = PlayerCharacter->GetEquippedShield())
        {
            Shield->DisableShieldBlock();
        }
    }
}

void UEldenCombatComponent::ExecuteParry()
{
    if (!PlayerCharacter || !CachedAnimInstance) return;
    if (PlayerCharacter)
    {
        if (PlayerCharacter->GetState() == ECharacterState::Dead ||
            PlayerCharacter->GetState() == ECharacterState::Rolling ||
            PlayerCharacter->GetState() == ECharacterState::Blocking)
        {
            return;
        }

        PlayerCharacter->SetState(ECharacterState::Parrying);
       

        if (BlockMontage)
        {
            CachedAnimInstance->Montage_Play(BlockMontage, 1.0f);
            
            FOnMontageEnded ParryEndDelegate;
            ParryEndDelegate.BindUObject(this, &UEldenCombatComponent::OnParryMontageEnded);
            CachedAnimInstance->Montage_SetEndDelegate(ParryEndDelegate, BlockMontage);
        }
    }
}

void UEldenCombatComponent::OnParryMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (PlayerCharacter)
    {
        // 상태를 다시 평소(Idle)로 복구
        PlayerCharacter->SetState(ECharacterState::Idle);
    }
}
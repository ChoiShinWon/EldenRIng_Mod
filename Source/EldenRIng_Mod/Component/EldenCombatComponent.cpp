// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/Component/EldenCombatComponent.h"
#include "GameFramework/Character.h" 
#include "EldenRing_Mod/Character/EldenCharacter.h"


UEldenCombatComponent::UEldenCombatComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

}



void UEldenCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}



void UEldenCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}


void UEldenCombatComponent::ProcessCombo()
{
    // 1. 소유주(캐릭터)의 메시로부터 애니메이션 인스턴스를 확실히 확보
    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (!OwnerChar || !OwnerChar->GetMesh()) return;

    UAnimInstance* AnimInstance = OwnerChar->GetMesh()->GetAnimInstance();
    if (!AnimInstance || !ComboMontage) return;

    bComboQueued = false;
    ComboCount++;

    FName SectionName = FName(*FString::Printf(TEXT("Attack%d"), ComboCount));

    if (!AnimInstance->Montage_IsPlaying(ComboMontage))
    {
        AnimInstance->Montage_Play(ComboMontage, 1.0f);
    }
    

    AnimInstance->Montage_JumpToSection(SectionName, ComboMontage);
    
    bIsAttacking = true;
    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(this, &UEldenCombatComponent::OnAttackMontageEnded);
    AnimInstance->Montage_SetEndDelegate(EndDelegate, ComboMontage);
   
}

void UEldenCombatComponent::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    bIsAttacking = false;
    bComboQueued = false;
    ComboCount = 0;
    AEldenCharacter* EldenCharacter = Cast<AEldenCharacter>(GetOwner());
    if (EldenCharacter)
    {
        EldenCharacter->SetState(ECharacterState::Idle);
    }
}

void UEldenCombatComponent::SetComboWindow(bool bOpen)
{
    if (!bOpen && bComboQueued)
    {
        if (ComboCount >= 3)
        {
            bComboQueued = false; // 예약 폐기
            return;
        }
        
        bComboQueued = false;
        ProcessCombo();
    }
}

void UEldenCombatComponent::ExecuteAttack()
{
  
    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (!OwnerChar || !OwnerChar->GetMesh()) return;

    if (!bIsAttacking)
    {
        ProcessCombo();
    }
    else
    {
        if (ComboCount < 3)
        {
            bComboQueued = true;
        }
       
    }
}
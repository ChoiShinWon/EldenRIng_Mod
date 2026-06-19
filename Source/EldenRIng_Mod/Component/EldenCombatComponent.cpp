// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/Component/EldenCombatComponent.h"
#include "GameFramework/Character.h" 


UEldenCombatComponent::UEldenCombatComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEldenCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEldenCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UEldenCombatComponent::ProcessCombo()
{
    // 1. 소유주(캐릭터)의 메시로부터 애니메이션 인스턴스를 확실히 확보
    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (!OwnerChar || !OwnerChar->GetMesh()) return;

    UAnimInstance* AnimInstance = OwnerChar->GetMesh()->GetAnimInstance();
    if (!AnimInstance || !ComboMontage) return;

    // 2. 콤보 로직 (기존 캐릭터에 있던 코드를 컴포넌트 문법에 맞게 수정)
    if (ComboCount >= 3) return; // 3타 제한

    bIsAttacking = true;
    ComboCount++;

    FName SectionName = FName(*FString::Printf(TEXT("Attack%d"), ComboCount));

    if (AnimInstance->Montage_IsPlaying(ComboMontage))
    {
        FName CurrentSection = AnimInstance->Montage_GetCurrentSection(ComboMontage);
        AnimInstance->Montage_SetNextSection(CurrentSection, SectionName, ComboMontage);
    }
    else
    {
        // 첫 공격 시작
        AnimInstance->Montage_Play(ComboMontage, 1.0f);
        AnimInstance->Montage_JumpToSection(SectionName, ComboMontage);

        // 3. 델리게이트 바인딩!
        FOnMontageEnded EndDelegate;
        EndDelegate.BindUObject(this, &UEldenCombatComponent::OnAttackMontageEnded);
        AnimInstance->Montage_SetEndDelegate(EndDelegate, ComboMontage);
    }
}

void UEldenCombatComponent::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    bIsAttacking = false;
    bComboQueued = false;
    ComboCount = 0;
}

void UEldenCombatComponent::SetComboWindow(bool bOpen)
{
    if (!bOpen && bComboQueued)
    {
        bComboQueued = false;
        ProcessCombo();
    }
}

void UEldenCombatComponent::ExecuteAttack()
{
    UE_LOG(LogTemp, Warning, TEXT("ExecuteAttack 진입함!"));

    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (OwnerChar && OwnerChar->GetMesh())
    {
        
        ProcessCombo();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("OwnerChar가 NULL이거나 Mesh가 없습니다!"));
    }
}
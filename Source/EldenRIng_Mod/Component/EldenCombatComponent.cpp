// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/Component/EldenCombatComponent.h"
#include "GameFramework/Character.h" 
#include "Containers/Array.h"
#include "EldenRing_Mod/Weapon/EldenShield.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Weapon/EldenWeapon.h"
#include "EldenRing_Mod/Character/EldenEnemy.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"
#include "Engine/Engine.h"


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

	const TArray<UAnimMontage*>* ComboMontagesPtr = GetCurrentComboMontages();
	if (!ComboMontagesPtr) return;

	const TArray<UAnimMontage*>& ComboMontages = *ComboMontagesPtr;

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

	const TArray<UAnimMontage*>* ComboMontagesPtr = GetCurrentComboMontages();
	if (!ComboMontagesPtr) return;
	const TArray<UAnimMontage*>& ComboMontages = *ComboMontagesPtr;

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
	if (PlayerCharacter->GetEquippedWeapon() == nullptr) return;
	if (PlayerCharacter->GetEquippedWeapon()->GetWeaponStance() == EWeaponStance::TwoHanded) return;

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
	if (PlayerCharacter->GetEquippedWeapon() == nullptr) return;
	if (PlayerCharacter->GetEquippedWeapon()->GetWeaponStance() == EWeaponStance::TwoHanded) return;
 
    if (PlayerCharacter->GetState() == ECharacterState::Dead ||
        PlayerCharacter->GetState() == ECharacterState::Rolling ||
        PlayerCharacter->GetState() == ECharacterState::Blocking)
    {
         return;
    }

    PlayerCharacter->SetState(ECharacterState::Parrying);
       

    if (ParryMontage)
    {
        CachedAnimInstance->Montage_Play(ParryMontage, 1.0f);
            
        FOnMontageEnded ParryEndDelegate;
        ParryEndDelegate.BindUObject(this, &UEldenCombatComponent::OnParryMontageEnded);
        CachedAnimInstance->Montage_SetEndDelegate(ParryEndDelegate, ParryMontage);
    }
   
}



bool UEldenCombatComponent::TryDeflect(const FVector& HitLocation, AEldenEnemy* Attacker)
{
	if (!bParryWindowActive || !Attacker || !PlayerCharacter) return false;
	FVector Fwd = PlayerCharacter->GetActorForwardVector();
	FVector DirToHit = HitLocation - PlayerCharacter->GetActorLocation();
	DirToHit.Z = 0.f;
	DirToHit = DirToHit.GetSafeNormal();

	float DotToHit = FVector::DotProduct(Fwd, DirToHit);              // 정면에서 옴?
	float DotFacing = FVector::DotProduct(Fwd, Attacker->GetActorForwardVector());
	
	if (DotToHit <= 0.f || DotFacing >= ParryFacingDot) return false;
	
	Attacker->ApplyStun();
	
	if (ParryVFX)   UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParryVFX, HitLocation);
    if (ParrySound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), ParrySound, HitLocation);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);
	GetWorld()->GetTimerManager().SetTimer(HitStopTimerHandle, this,
	    &UEldenCombatComponent::ResetTimeDilation, 0.01f, false);

	return true;
}

void UEldenCombatComponent::OnParryMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (PlayerCharacter)
    {
        // 상태를 다시 평소(Idle)로 복구
        PlayerCharacter->SetState(ECharacterState::Idle);
    }
}

void UEldenCombatComponent::ResetTimeDilation()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
}

const TArray<UAnimMontage*>* UEldenCombatComponent::GetCurrentComboMontages() const
{
	if (!PlayerCharacter) return nullptr;
	AEldenWeapon* CurrentWeapon = PlayerCharacter->GetEquippedWeapon();
	if (!CurrentWeapon) return nullptr;

	return &CurrentWeapon->GetComboMontages();
}

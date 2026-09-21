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

// 방어(우클릭) 실행 함수
void UEldenCombatComponent::ExecuteBlock()
{
	// null 체크들
    if (!PlayerCharacter || !CachedAnimInstance) return;

	// 무기 미장착 상태에서 가드 로직을 태우면 이후 GetWeaponStance() 호출에서
	// 널 포인터 역참조가 나므로, 가드 조건 검사 전에 반드시 먼저 걸러야 함.
	if (PlayerCharacter->GetEquippedWeapon() == nullptr) return;

	// 대검은 양손이 무기를 쥐고 있어서 방패를 들 손이 없다는 설정.
	// 두손 무기 장착 중엔 가드 자체를 시작하지 못하게 여기서 조기 리턴
	if (PlayerCharacter->GetEquippedWeapon()->GetWeaponStance() == EWeaponStance::TwoHanded) return;

    // 공격 중이거나 구르는 중이 아니면 가드 자세 진입 허용
    if (PlayerCharacter->GetState() == ECharacterState::Idle)
    {
		// 플레이어 상태 가드로 설정
        PlayerCharacter->SetState(ECharacterState::Blocking);

        // 필요하다면 방패 방어 히트박스 켜기
        if (AEldenShield* Shield = PlayerCharacter->GetEquippedShield())
        {
            Shield->EnableShieldBlock();
        }

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

// 패리 실행 함수
void UEldenCombatComponent::ExecuteParry()
{
	// null 체크들
    if (!PlayerCharacter || !CachedAnimInstance) return;
	if (PlayerCharacter->GetEquippedWeapon() == nullptr) return;
	// 패리도 방패로 받아치는 액션이므로 동일한 이유로 두손 무기일 땐 비활성화
	if (PlayerCharacter->GetEquippedWeapon()->GetWeaponStance() == EWeaponStance::TwoHanded) return;

	// 죽었거나 구르거나 가드중이 아니라면 패리 실행 불가
    if (PlayerCharacter->GetState() == ECharacterState::Dead ||
        PlayerCharacter->GetState() == ECharacterState::Rolling ||
        PlayerCharacter->GetState() == ECharacterState::Blocking)
    {
         return;
    }

	// 조건 통과하면 패리 상태로 Set
    PlayerCharacter->SetState(ECharacterState::Parrying);
       

	// 패리 몽타주
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

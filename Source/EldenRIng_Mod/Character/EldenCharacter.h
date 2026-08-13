// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EldenCharacter.generated.h"

// 전방 선언
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UEldenStatComponent;
class UEldenCombatComponent;
class ULockOnComponent;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
    Attacking UMETA(DisplayName = "Attacking"),
	Rolling UMETA(DisplayName = "Rolling"),
	Dead UMETA(DisplayName = "Dead"),
	Interacting UMETA(DisplayName = "Interact")
};

UCLASS()
class ELDENRING_MOD_API AEldenCharacter : public ACharacter
{
	GENERATED_BODY()

	

protected:
	virtual void BeginPlay() override;



	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State")
	ECharacterState CharacterState = ECharacterState::Idle;
	
	/*=============================================================================
	 * Camera
	 *=============================================================================*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;
	
	/*=============================================================================
	 * UI
	 *=============================================================================*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> HUDWidgetClass;
	
	// 생성된 위젯을 저장할 포인터
	UPROPERTY()
	class UEldenHUDWidget* CurrentHUD;
	
	/*=============================================================================
	 * Enhanced Input 
	 *=============================================================================*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* DodgeAction;

	// IA_Attack을 넣을 바구니
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LockOn")
	class UInputAction* LockOnAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* InteractAction;

	
	// 키보드/마우스에서 신호가 들어왔을 때 실행될 함수들
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
	void InteractButtonPressed();

	// Shift 키를 누를때와 뗄 때 실행될 함수
	void StartSprint();
	void StopSprint();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* RollMontage;
	
	FVector2D LastMoveInput;
	
	/*=============================================================================
	 * Weapon (무기 시스템)
	 *=============================================================================*/
    // 에디터에서 장착할 무기 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AEldenWeapon> WeaponClass;
	
	// 실제로 월드에 스폰되어 내 손에 들려있는 무기를 가리키는 포인터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class AEldenWeapon* EquippedWeapon;
	
	/*=============================================================================
	 * 공격 시스템 (Combat)
	 *=============================================================================*/
	
	
	// 마우스 클릭시 실행할 함수
	void Attack();
	
	UFUNCTION()
	void OnRollMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	


protected:
	
	
	/*=============================================================================
	 * 스태미너 비용 설정 (Stamina Cost)
	 *=============================================================================*/
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float DodgeStaminaCost = 25.0f;
	
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float AttackStaminaCost = 15.0f;
	
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float SprintStaminaCost = 10.0f; 
	
	// 달리고 있는지 확인
	bool bIsSprinting = false;

	/*=============================================================================
	 * 락온 시스템 (Lock - On)
	 *=============================================================================*/



	/*=============================================================================
	 * 방어 및 회피 (Defense & Dodge)
	 *=============================================================================*/

	// 캐릭터가 현재 무적 상태인지 확인하는 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|State")
	bool bIsInvincible = false;

public:
	AEldenCharacter();

	void SetState(ECharacterState NewState);
	ECharacterState GetState() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TScriptInterface<class IInteractable> CurrentInteractableTarget;

	/*=============================================================================
	 * Components (컴포넌트)
	 *=============================================================================*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UEldenStatComponent* StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UEldenCombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class ULockOnComponent* LockOnComponent;
	
	// 캐릭터가 장착 중인 무기를 반환하는 함수
	FORCEINLINE class AEldenWeapon* GetEquippedWeapon() const { return EquippedWeapon ;}
	
	void Dodge();
	bool bDodgeQueued = false;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	
	FORCEINLINE bool GetIsDead() const { return GetState() == ECharacterState::Dead; }


	bool GetIsLockedOn() const;
	// 기본 데미지 처리 함수 오버라이드
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void ToggleLockOn();

	// 외부에서 무적 상태를 켜고 끌 수 있는 함수
	void SetInvincible(bool bState);

	void DebugLevelUpVigor();
	void DebugLevelUpEndurance();
	void DebugLevelUpStrength();
};

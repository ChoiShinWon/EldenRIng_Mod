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
class UPointLightComponent;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
    Attacking UMETA(DisplayName = "Attacking"),
	Rolling UMETA(DisplayName = "Rolling"),
	Blocking UMETA(DisplayName = "Blocking"),
	Parrying UMETA(DisplayName = "Parrying"),
	Dead UMETA(DisplayName = "Dead"),
	Interacting UMETA(DisplayName = "Interact"),
	Damaged UMETA(DisplayName = "Damaged"),
	Drinking UMETA(DisplayName = "Drink")
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* UseItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SwitchItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* BlockAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ParryAction;

	

	void StartBlock();
	void StopBlock(); // 가드를 뗄 때 처리용
	void StartParry(); // 패리 시도용
	

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
	 * Weapon & Shield
	 *=============================================================================*/
    // 에디터에서 장착할 무기 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AEldenWeapon> WeaponClass;
	
	// 실제로 월드에 스폰되어 내 손에 들려있는 무기를 가리키는 포인터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class AEldenWeapon* EquippedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shield")
	TSubclassOf<class AEldenShield> ShieldClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shield")
	class AEldenShield* EquippedShield;

	
	
	/*=============================================================================
	 * 공격 시스템 (Combat)
	 *=============================================================================*/
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* HitReactMontage;
	
	// 마우스 클릭시 실행할 함수
	void Attack();
	
	UFUNCTION()
	void OnRollMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnHitReactMontageEnded(UAnimMontage* Montage, bool bInterrupted);


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

	// 역경직을 관리할 타이머 핸들
	FTimerHandle HitStopTimerHandle;

	// 느려진 시간을 다시 원상 복구 시키는 함수
	void ResetTimeDilation();

	/*=============================================================================
	 * 아이템 사용 (Item Usage)
	 *=============================================================================*/
	

	// 키보드를 눌렀을 때 실행할 함수
	void UseItem();

	void SwitchItem();

	void SetDrinkingVisuals(bool bDrinking);


	void OnPotionMontageEnded(UAnimMontage* Montage, bool bInterrupted);
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UEldenInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	class UPointLightComponent* DrinkLight;

	
	
	// 캐릭터가 장착 중인 무기를 반환하는 함수
	FORCEINLINE class AEldenWeapon* GetEquippedWeapon() const { return EquippedWeapon ;}
	
	void Dodge();
	bool bDodgeQueued = false;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE class AEldenShield* GetEquippedShield() const { return EquippedShield ;}
	
	FORCEINLINE bool GetIsDead() const { return GetState() == ECharacterState::Dead; }

	bool bShieldBlockedAttack = false;

	bool GetIsLockedOn() const;
	// 기본 데미지 처리 함수 오버라이드
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void ToggleLockOn();

	// 외부에서 무적 상태를 켜고 끌 수 있는 함수
	void SetInvincible(bool bState);

	void DebugLevelUpVigor();
	void DebugLevelUpEndurance();
	void DebugLevelUpStrength();

	// 노티파이에서 호출할 진짜 회복 함수
	UFUNCTION(BlueprintCallable, Category = "Item")
	void ApplyItemEffect();

	// 애니메이션 노티파이 (AN_ParryCheck)에서 호출할 패링 검사 함수
	void ParryCheck();

	/*=============================================================================
	 * 퍼펙트 패리 전용 이펙트 & 사운드
	 *=============================================================================*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Effect")
	class UParticleSystem* ParryVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Effect")
	class USoundBase* ParrySound;

};

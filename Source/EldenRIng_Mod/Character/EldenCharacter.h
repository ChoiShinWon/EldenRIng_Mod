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


UCLASS()
class ELDENRING_MOD_API AEldenCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEldenCharacter();
	
	// 캐릭터가 장착 중인 무기를 반환하는 함수
	FORCEINLINE class AEldenWeapon* GetEquippedWeapon() const { return EquippedWeapon ;}
	
	void Dodge();
	

protected:
	virtual void BeginPlay() override;
	
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LockOn")
	class UInputAction* LockOnAction;
	
	// 키보드/마우스에서 신호가 들어왔을 때 실행될 함수들
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
	
	// Shift 키를 누를때와 뗄 때 실행될 함수
	void StartSprint();
	void StopSprint();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* RollMontage;
	
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bIsRolling = false;
	
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
	// 현재 콤보 번호
	int32 ComboCount = 0;
	
	// 다음 콤보가 예약되었는지 확인하는 플래그
	bool bComboQueued = false;
	
	// 콤보 공격용 몽타주 섹션이 3개로 나뉘어 있어야함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* ComboMontage;
	
	// 콤보 시작 진행 횟수
	void ProcessCombo();
	
	// 콤보 창 제어용 함수 (애님 노티파이에서 호출)
	UFUNCTION(BlueprintCallable)
	void SetComboWindow(bool bOpen);
	
	// IA_Attack을 넣을 바구니
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* AttackAction;
	
	
	// 마우스 클릭시 실행할 함수
	void Attack();
	
	// 공격 중인지 판별하는 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking;
	
	// 몽타주가 끝났을 때 엔진이 호출해 줄 함수
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnRollMontageEnded(UAnimMontage* Montage, bool bInterrupted);

protected:
	/*=============================================================================
	 * 체력 시스템 (Health)
	 *=============================================================================*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	bool bIsDead = false;

public:
	// HUD 에서 값을 읽어갈 수 있도록 Getter 추가
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE bool GetIsDead() const { return bIsDead; }
	FORCEINLINE bool GetIsLockedOn() const { return LockedTarget != nullptr; }

	// 기본 데미지 처리 함수 오버라이드
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	/*=============================================================================
	 * 스태미너 시스템 (Stamina)
	 *=============================================================================*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float MaxStamina = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float CurrentStamina = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRegenRate = 20.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRegenDelay = 1.5f;
	
	bool bCanRegen = true;
	
	FTimerHandle RegenDelayTimerHandle;
	
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

protected:
	/*=============================================================================
	 * 락온 시스템 (Lock - On)
	 *=============================================================================*/
	// 현재 락온된 몬스터를 가리키는 포인터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn")
	class AEldenEnemy* LockedTarget;

	// 락온 버튼을 눌렀을 때 실행될 함수
	void ToggleLockOn();


public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 스태미너 소모 함수
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void ConsumeStamina(float Amount);
	
	// 회복 재개 함수
	void ResetRegen();
	
	// HUD에서 값을 읽어갈 수 있도록 Getter 추가
	FORCEINLINE float GetCurrentStamina() const { return CurrentStamina; }
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }
};

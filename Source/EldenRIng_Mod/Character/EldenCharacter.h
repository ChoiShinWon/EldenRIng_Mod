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
class AEldenCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDiedDelegate, AEldenCharacter*, DeadPlayer);

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
    Attacking UMETA(DisplayName = "Attacking"),
	Rolling UMETA(DisplayName = "Rolling"),
	Blocking UMETA(DisplayName = "Blocking"),
	Parrying UMETA(DisplayName = "Parrying"),
	UsingSkill UMETA(DisplayName = "UsingSkill"),
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

	// 에디터에서 EldenMenuWidget BP 지정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> InventoryMenuWidgetClass;

	UPROPERTY()
	class UEldenMenuWidget* InventoryMenuWidget;
	
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
	UInputAction* SwitchWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SwitchShieldAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* BlockAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* FKeyAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleMenuAction;

	

	void StartBlock();
	void StopBlock(); // 가드를 뗄 때 처리용
	void StartParryOrSkill(); // 패리, 스킬 시도용
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LockOn")
	class UInputAction* LockOnAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* InteractAction;

	void InteractButtonPressed();
	
	// 키보드/마우스에서 신호가 들어왔을 때 실행될 함수들
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
	// Shift 키를 누를때와 뗄 때 실행될 함수
	void StartSprint();
	void StopSprint();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* RollMontage;
	
	FVector2D LastMoveInput;
	

	
	
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


	FVector  MeshDefaultRelLoc;
	FRotator MeshDefaultRelRot;
	FVector MeshDefaultRelScale;
	FName    MeshDefaultProfile;
	void HandleDeath();

	/*=============================================================================
	 * 스태미너 비용 설정 (Stamina Cost)
	 *=============================================================================*/
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float DodgeStaminaCost = 25.0f;
	
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float AttackStaminaCost = 15.0f;
	
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float SprintStaminaCost = 10.0f; 
	
	

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UEldenGraceRestComponent* GraceRestComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UEldenEquipmentComponent* EquipmentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UEldenInteractionComponent* InteractionComponent;

	void Revive(const FTransform&);
		
	void Dodge();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	FORCEINLINE bool GetIsDead() const { return GetState() == ECharacterState::Dead; }


	void StartAttackLunge(float Speed);
	void StopAttackLunge();


	bool GetIsLockedOn() const;
	// 기본 데미지 처리 함수 오버라이드
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void ToggleLockOn();

	// 은총에서 레벨업 UI를 여는 함수
	void OpenLevelUpMenu(TSubclassOf<class UUserWidget> WidgetClass);

	// 인벤토리 메뉴 여는 함수
	void ToggleInventoryMenu();

	// 외부에서 무적 상태를 켜고 끌 수 있는 함수
	void SetInvincible(bool bState);

	// HUD 보이기/ 감싸기
	void SetHUDVisible(bool bVisible);

#if WITH_EDITOR
	void DebugLevelUpVigor();
	void DebugLevelUpEndurance();
	void DebugLevelUpStrength();
#endif

	// 노티파이에서 호출할 진짜 회복 함수
	UFUNCTION(BlueprintCallable, Category = "Item")
	void ApplyItemEffect();

	FORCEINLINE class UEldenHUDWidget* GetCurrentHUD() const { return CurrentHUD; }


	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayerDiedDelegate OnPlayerDied;

	// HUD 장비 아이콘 갱신 헬퍼
	void RefreshEquipmentUI();

private:

	void StartDrinkingPotion();

};

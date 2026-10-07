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
	TSubclassOf<class UUserWidget> MenuWidgetClass;

	UPROPERTY()
	class UEldenMenuWidget* MenuWidget;
	
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

	
	void StartParryOrSkill(); // 패리, 스킬 시도용
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LockOn")
	class UInputAction* LockOnAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* InteractAction;

	
	// 키보드/마우스에서 신호가 들어왔을 때 실행될 함수들
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
	
	/*=============================================================================
	 * 공격 시스템 (Combat)
	 *=============================================================================*/
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* HitReactMontage;


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
	float AttackStaminaCost = 15.0f;
	

	/*=============================================================================
	 * 아이템 사용 (Item Usage)
	 *=============================================================================*/

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UEldenItemUseComponent* ItemUseComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UEldenLocomotionComponent* LocomotionComponent;

	void Revive(const FTransform&);

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	FORCEINLINE bool GetIsDead() const { return GetState() == ECharacterState::Dead; }



	bool GetIsLockedOn() const;
	// 기본 데미지 처리 함수 오버라이드
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	// 은총에서 레벨업 UI를 여는 함수
	void OpenLevelUpMenu(TSubclassOf<class UUserWidget> WidgetClass);

	// 인벤토리 메뉴 여는 함수
	void ToggleMenu();

	// 외부에서 무적 상태를 켜고 끌 수 있는 함수
	void SetInvincible(bool bState);

	// HUD 보이기/ 감싸기
	void SetHUDVisible(bool bVisible);

#if WITH_EDITOR
	void DebugLevelUpVigor();
	void DebugLevelUpEndurance();
	void DebugLevelUpStrength();
#endif

	void SetDrinkingVisuals(bool bDrinking);


	FORCEINLINE class UEldenHUDWidget* GetCurrentHUD() const { return CurrentHUD; }


	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayerDiedDelegate OnPlayerDied;

	// HUD 장비 아이콘 갱신 헬퍼
	void RefreshEquipmentUI();


};

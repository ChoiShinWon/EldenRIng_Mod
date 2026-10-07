
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "EldenRing_Mod/Component/EldenCombatComponent.h"
#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "EldenRing_Mod/Component/EldenEquipmentComponent.h"
#include "EldenRing_Mod/Component/LockOnComponent.h"
#include "EldenRing_Mod/Component/EldenGraceRestComponent.h"
#include "EldenRing_Mod/Component/EldenInteractionComponent.h"
#include "EldenRing_Mod/Component/EldenItemUseComponent.h"
#include "EldenRing_Mod/Component/EldenLocomotionComponent.h"
#include "EldenRing_Mod/EldenDamageEvent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "EldenRing_Mod/Weapon/EldenWeapon.h"
#include "EldenRing_Mod/Weapon/EldenShield.h"
#include "EldenRing_Mod/Widget/EldenHUDWidget.h"
#include "EldenRing_Mod/Widget/EldenMenuWidget.h"
#include "EldenRing_Mod/Character/EldenEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PointLightComponent.h"
#include "Components/CapsuleComponent.h"


AEldenCharacter::AEldenCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 스프링 암 생성 및 루트 컴포넌트에 부착
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // 카메라와 캐릭터 사이의 거리
	CameraBoom->bUsePawnControlRotation = true; // 마우스 움직임에 따라 셀카봉 회전

	// 카메라 생성 및 스프링 암 끝에 부착
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); //셀카봉 끝 소켓에 연결
	FollowCamera->bUsePawnControlRotation = false;

	DrinkLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("DrinkLight"));
	DrinkLight->SetupAttachment(GetMesh(), FName("LeftHandSocket"));
	DrinkLight->SetAttenuationRadius(80.0f);
	DrinkLight->SetIntensity(1.0f);
	DrinkLight->SetCastShadows(false); // 짧게 켜지는 연출용
	DrinkLight->SetVisibility(false); // 평소엔 꺼둠

	// 캐릭터 본체가 마우스 회전(컨트롤러)을 무조건 따라가지 않도록 분리
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 캐릭터가 걷거나 뛰는 방향(이동 방향)을 자연스럽게 바라보도록 설정
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// 캐릭터 스탯 컴포넌트 생성 
	StatComponent = CreateDefaultSubobject<UEldenStatComponent>(TEXT("StatComponent"));

	CombatComponent = CreateDefaultSubobject<UEldenCombatComponent>(TEXT("CombatComponent"));

	LockOnComponent = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOnComponent"));

	InventoryComponent = CreateDefaultSubobject<UEldenInventoryComponent>(TEXT("InventoryComponent"));

	// 축복 컴포넌트 생성
	GraceRestComponent = CreateDefaultSubobject<UEldenGraceRestComponent>(TEXT("GraceRestComponent"));

	EquipmentComponent = CreateDefaultSubobject<UEldenEquipmentComponent>(TEXT("EquipmentComponent"));

	InteractionComponent = CreateDefaultSubobject<UEldenInteractionComponent>(TEXT("InteractionComponent"));

	ItemUseComponent = CreateDefaultSubobject<UEldenItemUseComponent>(TEXT("ItemUseComponent"));

	LocomotionComponent = CreateDefaultSubobject<UEldenLocomotionComponent>(TEXT("LocomotionComponent"));
}

void AEldenCharacter::SetState(ECharacterState NewState)
{
	if (GetState() == ECharacterState::Dead) return;
	CharacterState = NewState;
}

ECharacterState AEldenCharacter::GetState() const
{
	return CharacterState;
}

// Called when the game starts or when spawned
void AEldenCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 1. 내 캐릭터를 조종하는 PlayerController를 가져와서 IMC 등록
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}


	// 초기 룬 테스트 세팅
	if (StatComponent)
	{
		// 초기 룬 테스트 세팅
		StatComponent->CurrentRunes = 10000;
	}


	if (HUDWidgetClass)
	{
		CurrentHUD = CreateWidget<UEldenHUDWidget>(GetWorld(), HUDWidgetClass);
		if (CurrentHUD)
		{
			CurrentHUD->AddToViewport();
			RefreshEquipmentUI();
		}
	}

	MeshDefaultRelLoc = GetMesh()->GetRelativeLocation();
	MeshDefaultRelRot = GetMesh()->GetRelativeRotation();
	MeshDefaultProfile = GetMesh()->GetCollisionProfileName();
	MeshDefaultRelScale = GetMesh()->GetRelativeScale3D();

}


void AEldenCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 락온 기능
	if (LockOnComponent)
	{
		LockOnComponent->UpdateLockOn(DeltaTime);
	}


}

void AEldenCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 2. 입력 신호가 들어올 때 내 클래스의 Move, Look 함수와 묶어주기
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AEldenCharacter::Move);
		}
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AEldenCharacter::Look);
		}
		if (AttackAction)
		{
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, CombatComponent, &UEldenCombatComponent::ExecuteAttack);
		}
		if (BlockAction)
		{
			EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, CombatComponent, &UEldenCombatComponent::ExecuteBlock);
			EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, CombatComponent, &UEldenCombatComponent::EndBlock);
		}
		if (FKeyAction)
		{

			EnhancedInputComponent->BindAction(FKeyAction, ETriggerEvent::Started, this, &AEldenCharacter::StartParryOrSkill);
		}
		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, LocomotionComponent, &UEldenLocomotionComponent::StartSprint);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, LocomotionComponent, &UEldenLocomotionComponent::StopSprint);
		}
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AEldenCharacter::Jump);
		}
		if (DodgeAction)
		{
			EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, LocomotionComponent, &UEldenLocomotionComponent::Dodge);
		}

		if (LockOnAction)
		{
			EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started, LockOnComponent, &ULockOnComponent::ToggleLockOn);
		}

		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, InteractionComponent, &UEldenInteractionComponent::ExecuteInteract);
		}

		if (SwitchItemAction)
		{
			EnhancedInputComponent->BindAction(SwitchItemAction, ETriggerEvent::Started, ItemUseComponent, &UEldenItemUseComponent::SwitchItem);
		}

		if (SwitchWeaponAction)
		{
			EnhancedInputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Started, EquipmentComponent, &UEldenEquipmentComponent::SwitchWeapon);
		}

		if (UseItemAction)
		{
			EnhancedInputComponent->BindAction(UseItemAction, ETriggerEvent::Started, ItemUseComponent, &UEldenItemUseComponent::UseItem);
		}

		if (ToggleMenuAction)
		{
			EnhancedInputComponent->BindAction(ToggleMenuAction, ETriggerEvent::Started, this, &AEldenCharacter::ToggleMenu);
		}

		if (SwitchShieldAction)
		{
			EnhancedInputComponent->BindAction(SwitchShieldAction, ETriggerEvent::Started, EquipmentComponent, &UEldenEquipmentComponent::SwitchShield);

		}

#if WITH_EDITOR
		PlayerInputComponent->BindKey(EKeys::One, IE_Pressed, this, &AEldenCharacter::DebugLevelUpVigor);
		PlayerInputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AEldenCharacter::DebugLevelUpEndurance);
		PlayerInputComponent->BindKey(EKeys::Three, IE_Pressed, this, &AEldenCharacter::DebugLevelUpStrength);
#endif
	}

}


void AEldenCharacter::StartParryOrSkill()
{
	if (!CombatComponent) return;
    if (EquipmentComponent->GetEquippedShield() && !EquipmentComponent->GetEquippedShield()->IsHidden())
    {
		CombatComponent->ExecuteParry();
	}	
	else
	{
		CombatComponent->ExecuteWeaponSkill();
	}

}

// 이동 및 회전 로직
void AEldenCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	LocomotionComponent->LastMoveInput = MovementVector;

	if (GetState() != ECharacterState::Idle && GetState() != ECharacterState::Blocking
		&& GetState() != ECharacterState::Drinking) return;


	if (Controller != nullptr)
	{
		// 카메라가 바라보는 방향을 가져와서 Pitch, Roll 무시하고 평면(Yaw) 방향만 추출
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 그 방향을 기준으로 앞과 오른쪽이 어디인지 절대 벡터로 계산
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 캐릭터에 힘 가하기
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AEldenCharacter::Look(const FInputActionValue& Value)
{
	if (GetState() != ECharacterState::Idle) return;

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}



void AEldenCharacter::OpenLevelUpMenu(TSubclassOf<class UUserWidget> WidgetClass)
{
	if (WidgetClass)
	{
		UUserWidget* LevelUpWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
		if (LevelUpWidget)
		{
			LevelUpWidget->AddToViewport();

			if (APlayerController* PC = Cast < APlayerController>(GetController()))
			{
				PC->bShowMouseCursor = true;
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(LevelUpWidget->TakeWidget());
				PC->SetInputMode(InputMode);
			}
		}

		GetCharacterMovement()->StopMovementImmediately();
		SetState(ECharacterState::Interacting);
	}
}

void AEldenCharacter::ToggleMenu()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	// 메뉴창이 켜져있을 때
	if (MenuWidget)
	{
		MenuWidget->RemoveFromParent();
		MenuWidget = nullptr;

		if (PC)
		{
			PC->bShowMouseCursor = false;
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
		}

		SetState(ECharacterState::Idle);
		// 메뉴 창 닫을 때 게임 재개
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	}
	else
	{
		UEldenMenuWidget* NewMenuWidget = CreateWidget<UEldenMenuWidget>(GetWorld(), MenuWidgetClass);
		if (!NewMenuWidget) return;
		NewMenuWidget->InitMenu(InventoryComponent, StatComponent, EquipmentComponent);
		MenuWidget = NewMenuWidget;
		MenuWidget->AddToViewport();
		if (PC)
		{
			PC->bShowMouseCursor = true;
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(NewMenuWidget->TakeWidget());
			InputMode.SetHideCursorDuringCapture(false);
			PC->SetInputMode(InputMode);
		}
		GetCharacterMovement()->StopMovementImmediately();
		SetState(ECharacterState::Interacting);
		// 메뉴 창 열 때 게임 정지
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0001f);
	}
}


void AEldenCharacter::Revive(const FTransform& SpawnTransform)
{
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionProfileName(MeshDefaultProfile);
	GetMesh()->AttachToComponent(GetCapsuleComponent(),
		FAttachmentTransformRules::KeepRelativeTransform);
	GetMesh()->SetRelativeLocationAndRotation(MeshDefaultRelLoc, MeshDefaultRelRot);
	GetMesh()->SetRelativeScale3D(MeshDefaultRelScale);
	// 위치 이동
	SetActorLocationAndRotation(
		SpawnTransform.GetLocation(),
		SpawnTransform.GetRotation().Rotator(),
		false, nullptr, ETeleportType::TeleportPhysics);
	// 무브먼트, 상태
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	CharacterState = ECharacterState::Idle;
	SetInvincible(false); // 빠뜨리면 부활 후 영구 무적

	// 리소스 풀 회복
	if (StatComponent) StatComponent->FullRestore();
	if (InventoryComponent) InventoryComponent->RefillPotions();

	// 부활하면 플레이어 HUD 다시 Visible
	if (CurrentHUD) CurrentHUD->SetVisibility(ESlateVisibility::Visible);
}


void AEldenCharacter::OnHitReactMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (GetState() == ECharacterState::Damaged)
	{
		SetState(ECharacterState::Idle);
	}
}

void AEldenCharacter::HandleDeath()
{
	if (GetState() == ECharacterState::Dead) return;

	SetState(ECharacterState::Dead);

	SetInvincible(true);
	GetCharacterMovement()->DisableMovement();
	GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);

	// 죽었을때 플레이어 HUD 숨김
	if (CurrentHUD)
	{
		CurrentHUD->SetVisibility(ESlateVisibility::Collapsed);
	}

	OnPlayerDied.Broadcast(this);
}

float AEldenCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 죽었다면 무시
	if (GetState() == ECharacterState::Dead) return 0.0f;

	EDamageResult Result;
	float FinalDamage = CombatComponent->ResolveIncomingDamage(DamageAmount, DamageCauser, Result);

	// 결과를 공격자에게 돌려주기
	if (DamageEvent.IsOfType(FEldenDamageEvent::ClassID))
	{
		static_cast<const FEldenDamageEvent&>(DamageEvent).Result = Result;
	}

	// 회피/패리면 데미지 처리 자체를 안한다
	if (Result == EDamageResult::Parried || Result == EDamageResult::Dodged) return 0.0f;

	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 계산된 데미지 적용 (방어에 성공했다면 FinalDamage가 0이므로 체력 안 깎임)
	if (StatComponent)
	{
		StatComponent->ApplyDamage(FinalDamage);
		float LeftHealth = StatComponent->GetCurrentHealth();
		UE_LOG(LogTemp, Warning, TEXT("데미지 적용됨! 남은 체력 : %f"), LeftHealth);

		if (LeftHealth <= 0.0f)
		{
			HandleDeath();
		}
		else if (FinalDamage > 0.0f)
		{
			SetState(ECharacterState::Damaged);
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				if (HitReactMontage)
				{
					AnimInstance->Montage_Play(HitReactMontage);

					FOnMontageEnded HitEndDelegate;
					HitEndDelegate.BindUObject(this, &AEldenCharacter::OnHitReactMontageEnded);
					AnimInstance->Montage_SetEndDelegate(HitEndDelegate, HitReactMontage);
				}
				else
				{
					AnimInstance->StopAllMontages(0.1f);
				}

			}
		}
	}

	return FinalDamage;
}


bool AEldenCharacter::GetIsLockedOn() const
{
	return LockOnComponent && LockOnComponent->HasTarget();
}


void AEldenCharacter::SetInvincible(bool bState)
{
	CombatComponent->bIsInvincible = bState;
}

void AEldenCharacter::SetHUDVisible(bool bVisible)
{
	if (CurrentHUD)
	{
		if (bVisible) CurrentHUD->SetVisibility(ESlateVisibility::Visible);
		else CurrentHUD->SetVisibility(ESlateVisibility::Collapsed);
	}
}

#if WITH_EDITOR
void AEldenCharacter::DebugLevelUpVigor()
{
	StatComponent->LevelUpStat(EEldenStatType::Vigor);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("생명력 증가!"));
}

void AEldenCharacter::DebugLevelUpEndurance()
{
	StatComponent->LevelUpStat(EEldenStatType::Endurance);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("지구력 증가!"));
}

void AEldenCharacter::DebugLevelUpStrength()
{
	StatComponent->LevelUpStat(EEldenStatType::Strength);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("공격력 증가!"));
}
#endif

/*=============================================================================
 * 포션 로직 구현부
 *=============================================================================*/

void AEldenCharacter::SetDrinkingVisuals(bool bDrinking)
{
	if (bDrinking && DrinkLight && InventoryComponent)
	{
		DrinkLight->SetLightColor(InventoryComponent->GetCurrentDrinkGlowColor());
	} 

	if (DrinkLight) DrinkLight->SetVisibility(bDrinking);
	if (EquipmentComponent->GetEquippedWeapon() && EquipmentComponent->GetEquippedWeapon()->GetWeaponStance() == EWeaponStance::TwoHanded) return;
	if (EquipmentComponent->GetEquippedShield()) EquipmentComponent->GetEquippedShield()->SetActorHiddenInGame(bDrinking);
}

void AEldenCharacter::RefreshEquipmentUI()
{
	if (!CurrentHUD) return;
	UTexture2D* WeaponTexture = nullptr;
	UTexture2D* ShieldTexture = nullptr;
	FString CurrentSkillName = TEXT("");

	if (EquipmentComponent->GetEquippedWeapon())
	{
		WeaponTexture = EquipmentComponent->GetEquippedWeapon()->GetIcon();
		CurrentSkillName = EquipmentComponent->GetEquippedWeapon()->GetSkillName();
	}
	// 방패를 장착하고 있는가가 아니라, 지금 화면에 방패가 보이는가를 기준으로 UI 갱신
	// EquippedShield 포인터 자체는 두손 무기 장착 중에도 계속 살아있음
	// SetActorHiddenInGame만 했지 슬롯에서 빼거나 nullptr로 비운게 아니기 때문
	// 포인터 유무만 따지면 두손 무기 장착 중에도 방패 UI가 보이기 때문에 IsHidden() 체크
	if (EquipmentComponent->GetEquippedShield() && !EquipmentComponent->GetEquippedShield()->IsHidden())
	{
		ShieldTexture = EquipmentComponent->GetEquippedShield()->GetIcon();
		CurrentSkillName = EquipmentComponent->GetEquippedShield()->GetSkillName();
	}
	CurrentHUD->UpdateEquipmentUI(WeaponTexture, ShieldTexture,
		InventoryComponent ? InventoryComponent->GetCurrentItemIcon() : nullptr, CurrentSkillName);
}


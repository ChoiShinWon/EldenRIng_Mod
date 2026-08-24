
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "EldenRing_Mod/Component/EldenCombatComponent.h"
#include "EldenRing_Mod/Component/LockOnComponent.h"
#include "EldenRing_Mod/Interface/Interactable.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "EldenRing_Mod/Weapon/EldenWeapon.h"
#include "EldenRing_Mod/Weapon/EldenShield.h"
#include "EldenRing_Mod/Widget/EldenHUDWidget.h"
#include "EldenRing_Mod/StatUtils.h"
#include "EldenRing_Mod/Character/EldenEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


AEldenCharacter::AEldenCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// 1. 스프링 암 생성 및 루트 컴포넌트에 부착
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // 카메라와 캐릭터 사이의 거리
	CameraBoom->bUsePawnControlRotation = true; // 마우스 움직임에 따라 셀카봉 회전
	
	// 2. 카메라 생성 및 스프링 암 끝에 부착
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); //셀카봉 끝 소켓에 연결
	FollowCamera->bUsePawnControlRotation = false;

	// 3. 캐릭터 본체가 마우스 회전(컨트롤러)을 무조건 따라가지 않도록 분리
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// 4. 캐릭터가 걷거나 뛰는 방향(이동 방향)을 자연스럽게 바라보도록 설정
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// 캐릭터 스탯 컴포넌트 생성 
	StatComponent = CreateDefaultSubobject<UEldenStatComponent>(TEXT("StatComponent"));

	CombatComponent = CreateDefaultSubobject<UEldenCombatComponent>(TEXT("CombatComponent"));
	
	LockOnComponent = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOnComponent"));
}

void AEldenCharacter::SetState(ECharacterState NewState)
{
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
	
	// 무기 스폰 및 장착 로직
	// 에디터에서 무기 클래스를 칸에 제대로 넣었는지 확인
	if (WeaponClass != nullptr)
	{
		// 월드에 무기 액터 생성
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		EquippedWeapon = GetWorld()->SpawnActor<AEldenWeapon>(WeaponClass,
			GetActorLocation(),GetActorRotation(), SpawnParams);
		
		// 스폰 성공하면 손에 있는 소켓에 갖다 붙임
		if (EquippedWeapon != nullptr)
		{
			// 부착 규칙: 위치, 회전, 스케일 모두 소켓 따라가기
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
			// 무기의 루트 컴포넌트를 캐릭터 Mesh에 있는 RightHandSocket에 붙이기
			EquippedWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("RightHandSocket"));
		}
	}

	if (ShieldClass != nullptr)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		EquippedShield = GetWorld()->SpawnActor<AEldenShield>(ShieldClass,
			GetActorLocation(), GetActorRotation(), SpawnParams);

		if (EquippedShield != nullptr)
		{
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

			EquippedShield->AttachToComponent(GetMesh(), AttachmentRules, FName("LeftHandSocket"));
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
			UTexture2D* WeaponTexture = nullptr;
			UTexture2D* ShieldTexture = nullptr;
			FString CurrentSkillName = TEXT("");

			if (EquippedWeapon)
			{
				WeaponTexture = EquippedWeapon->GetIcon();
				CurrentSkillName = EquippedWeapon->GetSkillName();
			}
			if (EquippedShield)
			{
				ShieldTexture = EquippedShield->GetIcon();
				CurrentSkillName = EquippedShield->GetSkillName();
			}
			CurrentHUD->UpdateEquipmentUI(WeaponTexture, ShieldTexture, nullptr, CurrentSkillName);
		}
	}

	
}


// Called every frame
void AEldenCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsSprinting && GetVelocity().Size() > 0.0f)
	{
		StatComponent->ConsumeStamina(SprintStaminaCost * DeltaTime);
		
		// 달리다가 스태미너 떨어지면 멈춤
		if (StatComponent->CurrentStamina <= 0.0f)
		{
			StopSprint();
		}
	}
	
	// 회복 가능 상태이고 최대치가 아니면 매 프레임 회복시킴
	if (StatComponent->bCanRegen && StatComponent->CurrentStamina < StatComponent->MaxStamina)
	{
		StatComponent->CurrentStamina = FMath::Clamp(StatComponent->CurrentStamina + (StatComponent->StaminaRegenRate * DeltaTime), 0.0f, StatComponent->MaxStamina);
	}

	// 락온 기능
	if (LockOnComponent)
	{
		LockOnComponent->UpdateLockOn(DeltaTime);
	}

}

// Called to bind functionality to input
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
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AEldenCharacter::Attack);
		}
		if (BlockAction)
		{
			EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &AEldenCharacter::StartBlock);
			EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &AEldenCharacter::StopBlock);
		}
		if (ParryAction)
		{
			
			EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Started, this, &AEldenCharacter::StartParry);
		}
		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this , &AEldenCharacter::StartSprint);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AEldenCharacter::StopSprint);
		}
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AEldenCharacter::Jump);
		}
		if (DodgeAction)
		{
			EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &AEldenCharacter::Dodge);
		}

		if (LockOnAction)
		{
			EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started, this, &AEldenCharacter::ToggleLockOn);
		}

		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AEldenCharacter::InteractButtonPressed);
		}

		PlayerInputComponent->BindKey(EKeys::One, IE_Pressed, this, &AEldenCharacter::DebugLevelUpVigor);
		PlayerInputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AEldenCharacter::DebugLevelUpEndurance);
		PlayerInputComponent->BindKey(EKeys::Three, IE_Pressed, this, &AEldenCharacter::DebugLevelUpStrength);
	}

}

void AEldenCharacter::StartBlock()
{

	if (CombatComponent)
	{
		CombatComponent->ExecuteBlock();
	}
}

void AEldenCharacter::StopBlock()
{
	if (CombatComponent)
	{
		CombatComponent->EndBlock(); // 가드 해제 함수
	}
}

void AEldenCharacter::StartParry()
{
	if (CombatComponent)
	{
		CombatComponent->ExecuteParry(); 
	}
}

// 이동 및 회전 로직
void AEldenCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	LastMoveInput = MovementVector;

	if (GetState() != ECharacterState::Idle && GetState() != ECharacterState::Blocking) return;
	
	
	if (Controller != nullptr)
	{
		// 카메라가 바라보는 방향을 가져와서 Pitch, Roll 무시하고 평면(Yaw) 방향만 추출
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw,0);
		
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
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AEldenCharacter::InteractButtonPressed()
{
	if (CurrentInteractableTarget != nullptr)
	{
		CurrentInteractableTarget->Interact(this);
	}
}

void AEldenCharacter::ToggleLockOn()
{
	// 캐릭터는 입력을 받아서 컴포넌트에게 '전달(위임)'만 합니다.
	if (LockOnComponent)
	{
		LockOnComponent->ToggleLockOn();
	}
}

void AEldenCharacter::Dodge()
{
	// 스태미너가 부족거나 이미 구르는 중이라면 무시
	if (StatComponent->CurrentStamina < DodgeStaminaCost || GetState() == ECharacterState::Rolling) return;

	if (GetState() == ECharacterState::Attacking)
	{
		bDodgeQueued = true;
		if (CombatComponent)
		{
			CombatComponent->bComboQueued = false;
			CombatComponent->ComboCount = 0;
		}
		return;
	}

	// Idle일 때 즉시 구르기 실행
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && RollMontage)
	{
		//스태미너 소모 함수 호출
		StatComponent->ConsumeStamina(DodgeStaminaCost);
		SetState(ECharacterState::Rolling);

		
		// 구르기 시작할 때, 캐릭터가 이동 방향을 바라보도록 강제로 설정
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		bUseControllerRotationYaw = false;
		
		
		// 현재 캐릭터가 이동 중이던 방향(속도)을 가져옵니다. (WASD를 누르고 있으면 그 방향이 됨)
		FVector DodgeDir = GetVelocity().GetSafeNormal();


		//만약 제자리에 서서 구르기만 눌렀다면?
		if (DodgeDir.IsNearlyZero() && !LastMoveInput.IsNearlyZero())
		{
			if (Controller != nullptr)
			{
				const FRotator Rotation = Controller->GetControlRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);

				const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
				const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
				DodgeDir = (ForwardDirection * LastMoveInput.Y + RightDirection * LastMoveInput.X).GetSafeNormal();
			}
		}
		else if (DodgeDir.IsNearlyZero() && LastMoveInput.IsNearlyZero())
		{
			DodgeDir = GetActorForwardVector();
		}

		// 구를 방향으로 회전값 계산
		FRotator DodgeRotation = DodgeDir.Rotation();
		DodgeRotation.Pitch = 0.0f; // 바닥으로 처박히는 것 방지
		DodgeRotation.Roll = 0.0f;

		// 캐릭터 몸통을 즉시 강제로 돌려버림!
		SetActorRotation(DodgeRotation, ETeleportType::TeleportPhysics);

		// 몽타주 재생
		AnimInstance->Montage_Play(RollMontage);

		// --- 구르기 종료 감지 예약 ---
		FOnMontageEnded RollEndDelegate;
		RollEndDelegate.BindUObject(this, &AEldenCharacter::OnRollMontageEnded);
		AnimInstance->Montage_SetEndDelegate(RollEndDelegate, RollMontage);
	}
}

// 구르기가 끝나면 호출되는 함수
void AEldenCharacter::OnRollMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	SetState(ECharacterState::Idle);
	
	if (LockOnComponent->HasTarget())
	{
		// 락온 중이었다면 다시 적을 노려보게 복구
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		// 평소 모드 복구
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

void AEldenCharacter::OnHitReactMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (GetState() == ECharacterState::Damaged)
	{
		SetState(ECharacterState::Idle);
	}
}

float AEldenCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (GetState() == ECharacterState::Dead) return 0.0f;
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	bShieldBlockedAttack = false;
	

	// 가드 상태이고 공격자가 있을 때만 각도 계산
	if (GetState() == ECharacterState::Blocking && DamageCauser != nullptr)
	{
		// 적중 방향 계산 (적 -> 나)
		FVector DamageDir = (DamageCauser->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		// 캐릭터 정면 벡터와 내적 (180도 이내면 양수)
		float DotToEnemy = FVector::DotProduct(GetActorForwardVector(), DamageDir);

		// 정면에서 날아온 공격만 방어 성공 (뒤통수 맞으면 가드 무효)
		if (DotToEnemy > 0.0f)
		{
			// 방어 시 소모할 스태미나 양 (기획에 따라 공격력의 50%로 설정)
			float StaminaCost = DamageAmount * 0.5f;

			if (StatComponent && StatComponent->CurrentStamina >= StaminaCost)
			{
				//  방어 성공: 스태미나만 깎이고 데미지는 0
				StatComponent->ConsumeStamina(StaminaCost);
				ActualDamage = 0.0f;
				bShieldBlockedAttack = true; // 무기에게 "방어 성공함" 신호를 보냄!

				UE_LOG(LogTemp, Warning, TEXT("🛡 가드 성공! 데미지 0, 스태미나 소모: %f"), StaminaCost);
			}
			else if (StatComponent)
			{
				// 가드 붕괴(Guard Break): 스태미나가 0이 되며 가드가 강제로 풀림
				StatComponent->CurrentStamina = 0.0f;
				SetState(ECharacterState::Idle); // 가드 해제
				if (EquippedShield) EquippedShield->DisableShieldBlock(); // 방패 박스도 끄기

				UE_LOG(LogTemp, Error, TEXT(" 가드 붕괴! 데미지 100%% 관통!"));
			}
		}
	}

	// 계산된 데미지 적용 (방어에 성공했다면 ActualDamage가 0이므로 체력 안 깎임)
	if (StatComponent)
	{
		StatComponent->ApplyDamage(ActualDamage);
		float LeftHealth = StatComponent->GetCurrentHealth();
		UE_LOG(LogTemp, Warning, TEXT("데미지 적용됨! 남은 체력 : %f"), LeftHealth);

		if (LeftHealth <= 0.0f)
		{
			SetState(ECharacterState::Dead);
			UE_LOG(LogTemp, Warning, TEXT("죽었다!"));
		}
		else if (ActualDamage > 0.0f)
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

	return ActualDamage;
}


void AEldenCharacter::StartSprint()
{
	if (StatComponent->CurrentStamina > 0.0f)
	{
		bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = 800.0f;
	}
} 

void AEldenCharacter::StopSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
}

void AEldenCharacter::Attack()
{
	
	if (GetState()== ECharacterState::Rolling || GetState() == ECharacterState::Dead) return;
	
	
	if (CombatComponent)
	{
		
		CombatComponent->ExecuteAttack();
	}

}

bool AEldenCharacter::GetIsLockedOn() const
{
	return LockOnComponent && LockOnComponent->HasTarget();
}



void AEldenCharacter::SetInvincible(bool bState)
{
	bIsInvincible = bState;
}

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

void AEldenCharacter::ParryCheck()
{
	// 현재 상태가 패링 시도중일 때만 작동
	if (GetState() != ECharacterState::Parrying) return;

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Parry), false, this);

	// 전방 스윕 거리와 구체 두께 (무기 길이 맞춰서 조절 가능)
	const float ParryRange = 200.f;
	const float ParryRadius = 80.0f;

	const FVector Start = GetActorLocation();
	const FVector End = Start + GetActorForwardVector() * ParryRange;

	// 내 캐릭터 앞쪽으로 구체를 쏴서 적이 있는지 검사
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults, Start, End, FQuat::Identity,
		ECollisionChannel::ECC_Pawn,
		FCollisionShape::MakeSphere(ParryRadius),
		Params);

	if (!bHit) return;

	for (const FHitResult& Result : HitResults)
	{
		if (AEldenEnemy* Enemy = Cast<AEldenEnemy>(Result.GetActor()))
		{
			// 적이 패링 가능 상태 (ANS_ParryWindow 구간) 인가?
			if (!Enemy->bIsParryable) continue;

			// 적이 내 정면에 있는가
			FVector DirToEnemy = (Enemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			float DotToEnemy = FVector::DotProduct(GetActorForwardVector(), DirToEnemy);

			// 적이 나를 마주보고 있나 (뒤통수 / 옆구리 패링 방지)
			float DotFacing = FVector::DotProduct(GetActorForwardVector(), Enemy->GetActorForwardVector());

			// 적이 정면에 있고, 서로 마주보고 있다면 패링 성공
			if (DotToEnemy > 0.0f && DotFacing < 0.0f)
			{
				Enemy->ApplyStun();

				if (ParryVFX)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParryVFX, Enemy->GetActorLocation());
				}
				if (ParrySound)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), ParrySound, Enemy->GetActorLocation());
				}

				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("패링 성공! 적 스턴!"));
				}

				

				// 타격감을 위한 0.1초 멈춤 효과(역경직)
				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);

				GetWorld()->GetTimerManager().SetTimer(HitStopTimerHandle, this,
					&AEldenCharacter::ResetTimeDilation, 0.01f, false);
				break;
			}
		}
	}
}

// 시간을 원래대로 돌리는 함수 구현
void AEldenCharacter::ResetTimeDilation()
{
	// 게임 전체 속도를 다시 1.0 (정상 속도)으로 롤백
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
}
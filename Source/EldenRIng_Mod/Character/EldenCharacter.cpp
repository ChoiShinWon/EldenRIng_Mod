
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "EldenRing_Mod/Weapon/EldenWeapon.h"
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
	
	if (HUDWidgetClass)
	{
		CurrentHUD = CreateWidget<UEldenHUDWidget>(GetWorld(), HUDWidgetClass);
		if (CurrentHUD)
		{
			CurrentHUD->AddToViewport();
		}
	}
}


// Called every frame
void AEldenCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsSprinting && GetVelocity().Size() > 0.0f)
	{
		ConsumeStamina(SprintStaminaCost * DeltaTime);
		
		// 달리다가 스태미너 떨어지면 멈춤
		if (CurrentStamina <= 0.0f)
		{
			StopSprint();
		}
	}
	
	// 회복 가능 상태이고 최대치가 아니면 매 프레임 회복시킴
	if (bCanRegen && CurrentStamina < MaxStamina)
	{
		CurrentStamina = FMath::Clamp(CurrentStamina + (StaminaRegenRate * DeltaTime), 0.0f, MaxStamina);
	}

	// 락온 기능
	if (LockedTarget)
	{
		// 타겟이 죽었거나 파괴되었다면 자동으로 락온 해제
		if (!IsValid(LockedTarget) || LockedTarget->GetIsDead())
		{
			LockedTarget->ShowTargetMark(false);
			ToggleLockOn();
		}
		else
		{
			// 내가 쳐다봐야 할 회전 값 계산 (내 위치-> 적 위치)
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockedTarget->GetActorLocation());

			// 카메라가 적의 발밑이 아니라 가슴팍을 보도록	
			LookAtRotation.Pitch -= 15.0f;

			// 현재 카메라 회전값
			FRotator CurrentRotation = Controller->GetControlRotation();

			// 보간
			FRotator SmoothRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, 5.0f);

			// 컨트롤러에 강제로 보간한 회전값 주입
			Controller->SetControlRotation(SmoothRotation);
		}
		
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
	}

}

// 이동 및 회전 로직
void AEldenCharacter::Move(const FInputActionValue& Value)
{
	if (bIsAttacking || bIsRolling || bIsDead) return; // 공격 중, 구르기 중, 사망 중에는 WASD 입력 차단
	FVector2D MovementVector = Value.Get<FVector2D>();
	
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

void AEldenCharacter::Dodge()
{
	if (bIsRolling || CurrentStamina < DodgeStaminaCost) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && RollMontage)
	{
		//스태미너 소모 함수 호출
		ConsumeStamina(DodgeStaminaCost);
		
		if (bIsAttacking)
		{
			AnimInstance->Montage_Stop(0.1f);
			bIsAttacking = false;
			bComboQueued = false;
			ComboCount = 0;
		}

		bIsRolling = true;

		
		// 구르기 시작할 때, 캐릭터가 이동 방향을 바라보도록 강제로 설정
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		bUseControllerRotationYaw = false;
		
		
		// 현재 캐릭터가 이동 중이던 방향(속도)을 가져옵니다. (WASD를 누르고 있으면 그 방향이 됨)
		FVector DodgeDir = GetVelocity().GetSafeNormal();

		//만약 제자리에 서서 구르기만 눌렀다면?
		if (DodgeDir.IsNearlyZero())
		{
			DodgeDir = GetActorForwardVector(); 
		}

		// 구를 방향으로 회전값 계산
		FRotator DodgeRotation = DodgeDir.Rotation();
		DodgeRotation.Pitch = 0.0f; // 바닥으로 처박히는 것 방지
		DodgeRotation.Roll = 0.0f;

		// 캐릭터 몸통을 즉시 강제로 돌려버림! (TeleportPhysics를 넣어서 물리 충돌 무시하고 즉시 휙 돌림)
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
	bIsRolling = false; // 이제 다시 구를 수 있는 상태로 변경
	
	if (LockedTarget)
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

float AEldenCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead) return 0.0f;

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 템플릿 사용해서 체력깎기
	FStatUtils::UpdateStat(CurrentHealth, MaxHealth, -ActualDamage);

	UE_LOG(LogTemp, Warning, TEXT("데미지 입음! 남은 체력 : %f"), CurrentHealth);

	if (CurrentHealth <= 0.0f)
	{
		bIsDead = true;
		UE_LOG(LogTemp, Warning, TEXT("죽었다!"));
		// 추후에 사망 애니메이션 추가

		if (LockedTarget) LockedTarget = nullptr; // 죽으면 락온 해제
	}
	return ActualDamage;
}

void AEldenCharacter::StartSprint()
{
	if (CurrentStamina > 0.0f)
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
	if (bIsAttacking)
	{
		// 다음 타수를 예약만 해두고 함수 종료
		if (!bComboQueued)
		{
			bComboQueued = true;
		}
		return;
	}
	
	// 공격중이 아니면 1타 시작
	ProcessCombo();
}


void AEldenCharacter::ProcessCombo()
{
	if (CurrentStamina < AttackStaminaCost) return;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (ComboMontage && AnimInstance)
	{
		// 1. 방어 코드: 3타까지만 가능하도록 제한
		if (ComboCount >= 3) return;
		
		// 공격 스태미너 소모
		ConsumeStamina(AttackStaminaCost);

		bIsAttacking = true;
		ComboCount++; // 1 -> 2 -> 3

		FName SectionName = FName(*FString::Printf(TEXT("Attack%d"), ComboCount));

		if (AnimInstance->Montage_IsPlaying(ComboMontage))
		{
			// 2. [완벽 해결] 강제 점프를 삭제하고, 현재 실제로 재생 중인 섹션 이름을 안전하게 가져옴
			FName CurrentSection = AnimInstance->Montage_GetCurrentSection(ComboMontage);
            
			// 3. 현재 섹션이 끝날 때 다음 섹션으로 자연스럽게 이어지도록 다리를 놓습니다.
			AnimInstance->Montage_SetNextSection(CurrentSection, SectionName, ComboMontage);
		}
		else 
		{
			// 공격 중이 아니면 1타부터 재생
			PlayAnimMontage(ComboMontage, 1.0f, SectionName);
           
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEldenCharacter::OnAttackMontageEnded);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, ComboMontage);
		}
	}
}

void AEldenCharacter::SetComboWindow(bool bOpen)
{
	// 콤보 창이 닫힐 때 예약된 콤보가 있다면 다음 타수로 넘어감
	if (!bOpen && bComboQueued)
	{
		bComboQueued = false;
		ProcessCombo();
	}
}

void AEldenCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsAttacking = false;
	bComboQueued = false;
	ComboCount = 0;
}



void AEldenCharacter::ConsumeStamina(float Amount)
{
	// 실제 스태미너 깎기
	CurrentStamina = FMath::Clamp(CurrentStamina - Amount, 0.0f, MaxStamina);
	
	// 회복 중단
	bCanRegen = false;
	
	// 타이머 재설정
	// 만약 1.5초 뒤에 회복하기로 예약했는데, 그 사이에 다시 구르면 기존 예약은 취소해야 하므로 Clear 하고 들어감.
	GetWorldTimerManager().ClearTimer(RegenDelayTimerHandle);
	// Clear후 다시 Timer 세팅, 시간이 StaminaRegen Delay후엔 Regen 다시 진행
	GetWorldTimerManager().SetTimer(RegenDelayTimerHandle, this, &AEldenCharacter::ResetRegen, StaminaRegenDelay, false);
	
}

void AEldenCharacter::ResetRegen()
{
	bCanRegen = true;
}

// 락온 시스템
void AEldenCharacter::ToggleLockOn()
{
	// 이미 락온중이라면 해제
	if (LockedTarget)
	{
		LockedTarget->ShowTargetMark(false);
		LockedTarget = nullptr;

		// 다시 원래대로 자연스럽게 달리도록 원상 복구
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		bUseControllerRotationYaw = false;
		return;
	}

	// 락온중이 아니라면
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEldenEnemy::StaticClass(), FoundEnemies);

	AEldenEnemy* ClosestEnemy = nullptr;
	float MinDistance = 1500.0f; // 탐색 반경 (너무 멀면 락온 안되도록)

	for (AActor* EnemyActor : FoundEnemies)
	{
		if (!IsValid(EnemyActor)) continue;

		AEldenEnemy* Enemy = Cast<AEldenEnemy>(EnemyActor);
		if (!Enemy) continue;

		// 살아있는 적 중에서 가장 가까운 놈 찾기
		if (!Enemy->GetIsDead())
		{
			float Distance = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				ClosestEnemy = Enemy;
			}
		}
	}

	if (ClosestEnemy)
	{
		LockedTarget = ClosestEnemy;
		LockedTarget->ShowTargetMark(true);
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		bUseControllerRotationYaw = true; // 카메라가 적을 보면 몸도 적을 향함
	}
}
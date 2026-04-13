
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "EldenRing_Mod/Weapon/EldenWeapon.h"


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
	
}

// Called every frame
void AEldenCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	}

}

// 이동 및 회전 로직
void AEldenCharacter::Move(const FInputActionValue& Value)
{
	if (bIsAttacking) return; // 공격중에는 WASD 입력 차단
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

void AEldenCharacter::StartSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = 800.0f;
}

void AEldenCharacter::StopSprint()
{
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
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (ComboMontage && AnimInstance)
	{
		// 1. 방어 코드: 3타까지만 가능하도록 제한
		if (ComboCount >= 3) return;

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

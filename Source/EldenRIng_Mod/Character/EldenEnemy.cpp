

#include "EldenRing_Mod/Character/EldenEnemy.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Component/EldenHitboxComponent.h"
#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "EldenRing_Mod/AI/EnemyAIController.h"
#include "EldenRing_Mod/StatUtils.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "AIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet/GameplayStatics.h>


AEldenEnemy::AEldenEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// 타겟 마크 위젯 컴포넌트 생성 및 설정
	TargetMarkWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TargetMarkWidget"));
	TargetMarkWidget->SetupAttachment(GetMesh());
	TargetMarkWidget->SetWidgetSpace(EWidgetSpace::Screen);
	TargetMarkWidget->SetDrawAtDesiredSize(true);
	TargetMarkWidget->SetVisibility(false);

	// 시야 컴포넌트	생성 및 설정
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SightRadius = 1500.0f; // 시야 반경 설정
	PawnSensingComp->SetPeripheralVisionAngle(60.0f); // 시야각 설정
	
	// 공격 박스 컴포넌트 생성 및 설정
	RightHandHitbox = CreateDefaultSubobject<UEldenHitboxComponent>(TEXT("RightHandHitbox"));
	RightHandHitbox->SetupAttachment(GetMesh(), FName("RightWeaponSocket"));

	LeftHandHitbox = CreateDefaultSubobject<UEldenHitboxComponent>(TEXT("LeftHandHitbox"));
	LeftHandHitbox->SetupAttachment(GetMesh(), FName("LeftWeaponSocket"));
}


void AEldenEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// 게임이 시작되면 현재 체력을 최대 체력으로 꽉 채워줌.
	CurrentHealth = MaxHealth;

	// 시야 컴포넌트가 유효하다면, 플레이어를 감지했을 때 호출될 함수를 바인딩
	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AEldenEnemy::OnSeePlayer);
	}

	/*AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		BlackboardComp = AIController-> GetBlackboardComponent();
	}*/

	
}


void AEldenEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	EnemyController = Cast<AEnemyAIController>(NewController);
	
}

void AEldenEnemy::OnSeePlayer(APawn* Pawn)
{
	// 플레이어를 감지했을 때의 로직 (예: 공격 시작, 애니메이션 재생 등)
	if (bHasAggro || bIsDead) return; // 이미 어그로가 있거나 죽은 상태라면 무시

	// 나를 본게 플레이어가 맞는지 체크
	if (Pawn)
	{
		bHasAggro = true;

		CombatTarget = Pawn;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (!bHasRoared)
		{
			bHasRoared = true;
			if (AnimInstance && AggroMontage)
			{
				AnimInstance->Montage_Play(AggroMontage);

				// 몽타주가 끝났을 때 호출될 델리게이트 설정
				FOnMontageEnded EndDelegate;
				EndDelegate.BindUObject(this, &AEldenEnemy::OnAggroMontageEnded);
				AnimInstance->Montage_SetEndDelegate(EndDelegate, AggroMontage);
			}

		}
		else
		{
			if (EnemyController)
			{
				EnemyController->SetAggroTarget(CombatTarget);
			}
		}

		

	}
}

void AEldenEnemy::OnAggroMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (EnemyController && CombatTarget)
	{
		EnemyController->SetAggroTarget(CombatTarget);
	}
}

void AEldenEnemy::PlayAttackMontage()
{
	// 공격 애니메이션 재생 함수. 공격 중이거나 죽은 상태라면 재생하지 않음.
	if (bIsAttacking || bIsDead || bIsStunned) return;
	

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	// 공격 애니메이션이 유효하다면 재생하고, 몽타주가 끝났을 때 호출될 델리게이트 설정
	if (AnimInstance && AttackMontage)
	{
		bIsAttacking = true;

		// 공격 애니메이션이 재생되는 동안에는 이동을 못하게 설정
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None); // 공격 중에는 이동 불가능하게 설정

		AnimInstance->Montage_Play(AttackMontage);	

		// 몽타주가 끝났을 때 호출될 델리게이트 설정
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &AEldenEnemy::OnAttackMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
	}
}

void AEldenEnemy::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 공격 애니메이션이 끝났을 때 호출되는 함수. 공격 상태를 false로 되돌려줌.
	bIsAttacking = false;

	// 공격이 끝나면 다시 이동 가능하게 설정
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking); // 공격이 끝나면 다시 이동 가능하게 설정
}	

float AEldenEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator,
	class AActor* DamageCauser)
{
	if (bIsDead) return 0.0f;

	// 공격을 받아 피가 깎인 순간, 나를 때린 녀석이 누구인지 CombatTarget에 저장
	if (EventInstigator && EventInstigator->GetPawn())
	{
		CombatTarget = EventInstigator->GetPawn();
	}

	// 부모 클래스의 기본 데미지 로직 실행
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// 스탯 처리 템플릿 적용 (FStatUtils)
	FStatUtils::UpdateStat(CurrentHealth, MaxHealth, -ActualDamage);

	
	if (CurrentHealth <= 0.0f)
	{
		Die();

	}
	else if (HitReactMontage && !bIsAttacking)
	{
		// 맞는 모션 재생
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(HitReactMontage);
		}
	}
	return ActualDamage;
}

void AEldenEnemy::ResetAggro()
{
	bHasAggro = false;

	CombatTarget = nullptr;


	if (EnemyController) 
	{
		EnemyController->ClearAggroTarget();

		// 멈추기
		EnemyController->StopMovement();
	}

}

void AEldenEnemy::Die()
{
	if (bIsDead) return; 
	bIsDead = true;

	if (EnemyController)
	{
		EnemyController->StopMovement();
		EnemyController->UnPossess();
	}

	AEldenCharacter* Player = Cast<AEldenCharacter>(CombatTarget);
	if (Player)
	{
		UEldenStatComponent* PlayerStat = Player->StatComponent;
		if (PlayerStat)
		{
			PlayerStat->AddRunes(RuneReward);

			if (GEngine)
			{
				FString Msg = FString::Printf(TEXT("룬 %d 개 획득!"), RuneReward);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, Msg);
			}
		} 
	}

	// 죽음 몽타주 재생
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);

		// 몽타주 종료 델리게이트 연결
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &AEldenEnemy::OnDeathMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, DeathMontage);
	}

	else
	{
		// 몽타주가 없다면 즉시 래그돌 (안전장치)
		OnDeathMontageEnded(nullptr, false);
	}

}

void AEldenEnemy::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 1. 캡슐 끄기 
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	GetMesh()->bPauseAnims = true;

	// 2. 메시(시체)의 콜리전 통제하기
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

	// 시체는 무조건 바닥(WorldStatic)과 움직이는 배경(WorldDynamic)만 부딪히게 설정
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore); // 일단 모든 충돌을 끔
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // 바닥/벽은 막음
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block); // 움직이는 물체 막음
	// (Pawn 채널은 Ignore 상태이므로 플레이어와 겹쳐도 날아가지 않음)

	// 3. 래그돌 켜기
	GetMesh()->SetSimulatePhysics(true);

	// 4. 삭제
	SetLifeSpan(5.0f);
}

void AEldenEnemy::EnableParryWindow()
{
	bIsParryable = true;
}

void AEldenEnemy::DisableParryWindow()
{
	bIsParryable = false;
}

void AEldenEnemy::ApplyStun()
{
	if (bIsDead || bIsStunned) return;

	bIsStunned = true;
	bIsAttacking = false;
	bIsParryable = false;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->StopAllMontages(0.1f);

		if (StunMontage)
		{
			AnimInstance->Montage_Play(StunMontage);
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEldenEnemy::OnStunMontageEnded);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, StunMontage);
		}
		
	}

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	if (EnemyController)
	{
		if (UBlackboardComponent* BB = EnemyController->GetBlackboardComponent())
		{
			BB->SetValueAsBool(FName("Stunned"), true);
		}
	}
}

void AEldenEnemy::OnStunMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsStunned = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	if (EnemyController)
	{
		if (UBlackboardComponent* BB = EnemyController->GetBlackboardComponent())
		{
			BB->SetValueAsBool(FName("Stunned"), false);
		}
	}
}

bool AEldenEnemy::IsTargetable() const
{
	return !bIsDead;
}

void AEldenEnemy::ShowTargetMark(bool bShow)
{
	if (TargetMarkWidget)
	{
		TargetMarkWidget->SetVisibility(bShow);
	}
}

void AEldenEnemy::EnableRightAttackCollision()
{
	if (RightHandHitbox) RightHandHitbox->EnableHitbox();
}

void AEldenEnemy::DisableRightAttackCollision()
{
	if (RightHandHitbox) RightHandHitbox->DisableHitbox();
}

void AEldenEnemy::EnableLeftAttackCollision()
{
	if (LeftHandHitbox) LeftHandHitbox->EnableHitbox();
}

void AEldenEnemy::DisableLeftAttackCollision()
{
	if (LeftHandHitbox) LeftHandHitbox->DisableHitbox();
}
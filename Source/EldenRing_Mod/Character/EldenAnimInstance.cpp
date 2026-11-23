

#include "EldenRing_Mod/Character/EldenAnimInstance.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Component/EldenEquipmentComponent.h"
#include "EldenRing_Mod/Weapon/EldenWeapon.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

void UEldenAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	// 게임 시작 시, 이 애니메이션을 입고 있는 주인을 찾아서 EldenCharacter로 캐스팅해서 저장
	EldenCharacter = Cast<AEldenCharacter>(TryGetPawnOwner());
}

void UEldenAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	
	// 에디터 프리뷰 창이거나 렉이 걸려서 주인을 놓쳤을 경우를 대비한 안전장치
	if (EldenCharacter == nullptr)
	{
		EldenCharacter = Cast<AEldenCharacter>(TryGetPawnOwner());
	}
	
	// 주인이 정상적으로 존재한다면, 매 프레임 상태를 훔쳐옴
	if (EldenCharacter)
	{
		const ECharacterState State = EldenCharacter->GetState();
		UEldenEquipmentComponent* Equipment = EldenCharacter->EquipmentComponent;
		CurrentWeaponStance = EWeaponStance::OneHanded;
		if (Equipment)
		{
			AEldenWeapon* Weapon = Equipment->GetEquippedWeapon();
			if (Weapon)
			{
				CurrentWeaponStance = Weapon->GetWeaponStance();
			}
		}

		// 속도 구하기
		FVector Velocity = EldenCharacter->GetVelocity();
		Velocity.Z = 0.0f; // 하늘에서 떨어지는 속도는 걷기와 뛰기 모션에 영향을 주면 안되므로 Z축은 0으로 날려버린다
		GroundSpeed = Velocity.Size(); // 평면 위를 굴러가는 순수한 이동 속도를 구함

		// 공중 상태 구하기
		// 캐릭터 무브먼트 컴포넌트에 너 지금 바닥 안 밟고 있어? 하고 물어보기
		UCharacterMovementComponent* Movement = EldenCharacter->GetCharacterMovement();
		if (Movement)
		{
			bIsFalling = Movement->IsFalling();
		}

		// 락온 상태 가져오기
		bIsLockedOn = EldenCharacter->GetIsLockedOn();

		// 이동 방향 구하기
		FRotator BaseRoation = EldenCharacter->GetActorRotation();
		Direction = UKismetAnimationLibrary::CalculateDirection(EldenCharacter->GetVelocity(), BaseRoation);


		bIsAttacking = State == ECharacterState::Attacking;

		bIsBlocking = State == ECharacterState::Blocking;

		// ----- 입력(가속도) 기준 판정 -----
		// 속도가 아니라 가속도를 보는 이유: 플레이어가 키를 누르고 있는 의도는 가속도에 즉시 반영되고,
		// 방향을 뒤집는 순간에도 0으로 가지 않는다. (속도는 -500 -> 0 -> +500 으로 0을 지난다.)
		bool bHasInput = false;
		float InputDirection = 0.0f;

		// 입력 여부 + 입력 방향
		if (Movement)
		{
			const FVector Accel = Movement->GetCurrentAcceleration();
			bHasInput = !Accel.IsNearlyZero();

			InputDirection = UKismetAnimationLibrary::CalculateDirection(Accel, BaseRoation);
		}

		// ----- 입력 유예 (키 교체 공백 메우기) -----
		// A를 떼고 D를 누를 때 입력이 1~3 프레임 0이 된다. 이때 오버레이가 켜지고 BlockSpeed가 꺾여서
		// 상체가 가드 Idle로 순간 돌아오던 문제를, 0.15초 동안은 "입력 있음"으로 취급해서 막는다.
		const float InputGraceTime = 0.15f;

		if (bHasInput)
		{
			TimeSinceLastInput = 0.0f;
			LastInputDirection = InputDirection;
		}
		else
		{
			TimeSinceLastInput += DeltaTime;
		}

		const bool bHasRecentInput = TimeSinceLastInput < InputGraceTime;

		const float LateralMinAngle = 60.0f;
		const float LateralMaxAngle = 120.0f;

		// 좌우 범위 판정
		// 60~120도 (좌우 +-90 근처)만 스트레이프로 본다. 대각선/앞/뒤는 일반 방어 자세를 유지한다.
		// 판정은 LastInputDirection 기준: 유예 구간에도 값이 유지돼야 판정이 깨지지 않는다.
		const float AbsDirection = FMath::Abs(LastInputDirection);
		const bool bIsLateralMove = AbsDirection >= LateralMinAngle && AbsDirection <= LateralMaxAngle;

		// ----- 방패 전용 블렌드 스페이스 입력 -----
		// 입력이 있는 동안은 속도를 500으로 고정해서, 감속 중 실제 속도가 낮아져도
		// 블렌드 스페이스가 (0,0)의 Idle을 섞지 않게 한다.
		const float BlockMinSpeed = 500.0f;

		BlockDirection = bHasRecentInput ? LastInputDirection : Direction;

		BlockSpeed = bHasRecentInput ? FMath::Max(GroundSpeed, BlockMinSpeed) : GroundSpeed;

		// 최종 판정
		const bool bIsLockedOnBlockStrafing = bIsBlocking && bIsLockedOn && bHasRecentInput && bIsLateralMove;


		// 좌우 스트레이프는 블렌드 스페이스 대신 ABP의 Blend Poses by Int가 포즈 단위로 크로스페이드한다.
		// (블렌드 스페이스를 거치면 -90 <-> +90 전환 때 중간의 Fwd/Bwd 샘플이 섞이기 때문)
		// 0 = 일반(BS_LockOn_Block), 1 = 좌(입력 방향 음수), 2 = 우(양수)
		BlockStrafeIndex = bIsLockedOnBlockStrafing ? (LastInputDirection < 0.0f ? 1 : 2) : 0;


		// 상체 IDLE 오버레이를 언제 꺼야 하는지가 아니라 언제 켜도 되는지를 화이트리스트로 정의
		// Attacking/Rolling/Damaged 등 풀바디 몽타주 상태를 하나씩 배제하는 대신
		// Idle(대검일 때만)이거나, Blocking일 때만 허용 -> 새 상태가 추가돼도 기본값이 안전하게 유지됨
		// 좌우 스트레이프 중에는 Strafe 클립이 상체까지 말도록 오버레이를 끈다.
		bShouldShowWeaponIdleOverlay =
			((State == ECharacterState::Idle && CurrentWeaponStance == EWeaponStance::TwoHanded) ||
				(bIsBlocking && !bIsLockedOnBlockStrafing));
	}
}

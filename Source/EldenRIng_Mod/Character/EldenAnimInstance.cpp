// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/Character/EldenAnimInstance.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
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
		// 속도 구하기
		FVector Velocity = EldenCharacter->GetVelocity();
		Velocity.Z = 0.0f; // 하늘에서 떨어지는 속도는 걷기와 뛰기 모션에 영향을 주면 안되므로 Z축은 0으로 날려버린다
		GroundSpeed = Velocity.Size(); // 평면 위를 굴러가는 순수한 이동 속도를 구함
		
		// 공중 상태 구하기
		// 캐릭터 무브먼트 컴포넌트에 너 지금 바닥 안 밟고 있어? 하고 물어보기
		bIsFalling = EldenCharacter->GetCharacterMovement()->IsFalling();

		// 락온 상태 가져오기
		bIsLockedOn = EldenCharacter->GetIsLockedOn();

		// 이동 방향 구하기
		FRotator BaseRoation = EldenCharacter->GetActorRotation();
		Direction = UKismetAnimationLibrary::CalculateDirection(EldenCharacter->GetVelocity(), BaseRoation);
	}
}

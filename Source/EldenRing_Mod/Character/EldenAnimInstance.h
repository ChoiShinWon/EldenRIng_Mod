// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EldenRing_Mod/Weapon/EldenWeapon.h"
#include "EldenAnimInstance.generated.h"

class AEldenCharacter;
enum class EWeaponStance : uint8;
/**
 * 
 */
UCLASS()
class ELDENRING_MOD_API UEldenAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// 블루프린트의 Event Blueprint Initialize Animation과 같은 역할
	virtual void NativeInitializeAnimation() override;

	// 블루프린트의 Event Blueprint Update Animation과 같은 역할 (매 프레임 호출)
	virtual void NativeUpdateAnimation(float DeltaTime) override;

protected:
	// 애님 그래프에서 사용할 변수들

	// 캐릭터의 현재 이동 속도 (평면 벡터 길이)
	UPROPERTY(BlueprintReadOnly, Category = "Character Status")
	float GroundSpeed = 0.f;

	// 캐릭터가 현재 공중에 떠 있는지
	UPROPERTY(BlueprintReadOnly, Category = "Character Status")
	bool bIsFalling = false;

	// 캐릭터 이동 방향
	UPROPERTY(BlueprintReadOnly, Category = "Character Status")
	float Direction = 0.f;

	// 방패(락온) 전용 값. BS_LockOn_Block / Blend Poses by Int에서만 쓴다
	// 일반 Direction/GroundSpeed는 "실제 속도"에서 계산돼서, 좌 <-> 우로 방향을 뒤집을 때
	// 속도가 0을 지나며 블렌드 스페이스가 Idle 샘플을 섞는다.
	// 그래서 입력(가속도) 기준으로 따로 만들고, 키 교체 공백은 유예 시간으로 메운다.
	UPROPERTY(BlueprintReadOnly, Category = "Character Status")
	float BlockDirection = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Character Status")
	float BlockSpeed = 0.f;

	// 방패 + 락온 좌우 스트레이프 구분 (ABP의 Blend Poses by Int 용)
	// 0 = 일반 (BS_LockOn_Block), 1 = 좌 스트레이프, 2 = 우 스트레이프
	// 좌우 스트레이프를 블렌드 스페이스 밖으로 뺀 이유: 블렌드 스페이스는 값이 이동하는 경로의
	// 모든 샘플을 섞어서, -90 -> +90 전환 때 중간의 Fwd/Bwd 클립이 비친다.
	// 포즈 단위 크로스페이드는 사이 클립이 끼지 않는다.
	UPROPERTY(BlueprintReadOnly, Category = "Character Status")
	int32 BlockStrafeIndex = 0;

	// 현재 락온 여부
	UPROPERTY(BlueprintReadOnly, Category = "Character Status")
	bool bIsLockedOn = false;

	UPROPERTY(BlueprintReadOnly, Category = "Character Status")
	bool bIsAttacking = false;

	UPROPERTY(BlueprintReadOnly, Category = "Character Status")
	bool bIsBlocking = false;

	UPROPERTY(BlueprintReadOnly, Category = "Character Status")
	bool bShouldShowWeaponIdleOverlay = false;

	UPROPERTY(BlueprintReadOnly, Category = "Character Status")
	EWeaponStance CurrentWeaponStance = EWeaponStance::OneHanded;


private:

	// 이 애님 인스턴스를 소유하고 있는 캐릭터 포인터 (캐스팅 비용 절약용)
	UPROPERTY()
	class AEldenCharacter* EldenCharacter = nullptr;

	// 입력 유예용. 키를 바꿀 때 입력이 1~3프레임 0이 되는 공백을 메우려고 쓴다.
	// 초기값을 유예 시간보다 크게 둔 이유: 0이면 시작 직후 입력 있음으로 오판해 모션이 튄다.
	float TimeSinceLastInput = 1.0f;

	// 마지막으로 눌렀던 입력 방향 (-180 ~ 180). 유예 구간에는 InputDirection이 0이라 이 값을 쓴다.
	float LastInputDirection = 0.f;
};

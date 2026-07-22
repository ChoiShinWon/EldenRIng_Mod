// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EldenAnimInstance.generated.h"

class AEldenCharacter;
class UEldenCombatComponent;
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
	float GroundSpeed;
	
	// 캐릭터가 현재 공중에 떠 있는지
	UPROPERTY(BlueprintReadOnly, Category = "Character Status")
	bool bIsFalling;

	// 캐릭터 이동 방향
	UPROPERTY(BlueprintReadOnly, Category = "Character Status")	
	float Direction;

	// 현재 락온 여부
	UPROPERTY(BlueprintReadOnly, Category = "Character Status")
	bool bIsLockedOn;

	UPROPERTY(BlueprintReadOnly, Category = "Character Status")
	bool bIsAttacking;
	
private:

	// 이 애님 인스턴스를 소유하고 있는 캐릭터 포인터 (캐스팅 비용 절약용)
	UPROPERTY()
	class AEldenCharacter* EldenCharacter;
};   

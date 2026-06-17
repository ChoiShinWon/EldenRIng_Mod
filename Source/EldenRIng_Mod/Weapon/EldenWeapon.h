// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EldenWeapon.generated.h"

UCLASS()
class ELDENRING_MOD_API AEldenWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEldenWeapon();
	
	// 무기의 박스 컴포넌트를 반환하는 함수
	FORCEINLINE class UBoxComponent* GetWeaponBox() const { return WeaponBox ;}
	
	void ClearHitActors() {AlreadyHitActors.Empty();}

	/*=============================================================================
	 * [미션 1] Anim Notify State에서 무기 판정을 켜고 끌 스위치 함수 선언하기
	 *=============================================================================*/
	 // 힌트 1: 나중에 만들 'AnimNotifyState' 클래스에서 이 무기를 꺼내서 호출해야 합니다. 
	 // 그렇다면 이 함수들은 public에 있어야 할까요, protected에 있어야 할까요?
	 // 
	 // [여기에 콜리전 켜기 함수 선언] (예: void EnableWeaponCollision(); )
	 // [여기에 콜리전 끄기 함수 선언]
	void EnableWeaponCollision();
	void DisableWeaponCollision();
protected:

	virtual void BeginPlay() override;
	
	// 무기 외형 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	UStaticMeshComponent* WeaponMesh;
	
	// 칼날에 씌울 충돌체
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class UBoxComponent* WeaponBox;
	
	// 박스에 무언가 닿았을 때(Overlap) 실행될 델리게이트 함수
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 이미 공격에 맞은 액터들을 저장하는 명단
	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;
	
	// 타격 시 터뜨릴 파티클 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Effects")
	class UParticleSystem* HitParticles;
	
	// 타격 시 재생할 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Effects")
	class USoundBase* HitSound;
	
};

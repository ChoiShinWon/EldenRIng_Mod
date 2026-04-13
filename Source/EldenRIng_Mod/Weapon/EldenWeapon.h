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
	
};

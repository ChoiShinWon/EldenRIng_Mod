// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EldenWeapon.generated.h"

class UEldenHitboxComponent;

UCLASS()
class ELDENRING_MOD_API AEldenWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEldenWeapon();
	

	/*=============================================================================
	 *  Anim Notify State에서 무기 판정을 켜고 끌 스위치 함수 선언하기
	 *=============================================================================*/
	 // [여기에 콜리전 켜기 함수 선언] 
	 // [여기에 콜리전 끄기 함수 선언]
	void EnableWeaponCollision();
	void DisableWeaponCollision();
protected:

	virtual void BeginPlay() override;
	
	// 무기 외형 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, Category = "UI")
	class UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, Category = "UI")
	FString SkillName;
	


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	float BaseDamage = 20.0f;

	// 칼날에 씌울 충돌체
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class UEldenHitboxComponent* WeaponHitbox;
	
public:
	FORCEINLINE class UTexture2D* GetIcon() const { return ItemIcon; }
	FORCEINLINE const FString& GetSkillName() const { return SkillName; }
};

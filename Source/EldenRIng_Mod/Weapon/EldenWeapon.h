// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimMontage.h"
#include "EldenWeapon.generated.h"

class UEldenHitboxComponent;
class USceneComponent;

UENUM(BlueprintType)
enum class EWeaponStance : uint8
{
	OneHanded UMETA(DisplayName = "OneHanded"),
	TwoHanded UMETA(DisplayName = "TwoHanded")
};

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	USceneComponent* WeaponRoot;
	
	// 무기 외형 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, Category = "UI")
	class UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, Category = "UI")
	FString SkillName;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stats")
	float BaseDamage = 20.0f;

	// 콤보 몽타주 배열
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Combat")
	TArray<UAnimMontage*> ComboMontages;

	// 스킬 전용 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Skill")
	class UAnimMontage* SkillMontage;

	// 스킬 데미지량
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Skill")
	float SkillDamage = 50.0f;

	// 스킬 포이즈 데미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Skill")
	float SkillPoiseDamage = 20.0f;

	// 스킬 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Skill")
	class UParticleSystem* SkillVFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Skill")
	int32 SkillVFXSpawnCount = 7;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Skill")
	float SkillTraceForwardOffset = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Skill")
	float SkillTraceLength = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Skill")
	FVector SkillTraceBoxExtent = FVector(50.f, 50.f, 50.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponStance WeaponStance = EWeaponStance::OneHanded;

	// 칼날에 씌울 충돌체
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class UEldenHitboxComponent* WeaponHitbox;
	
public:
	// 게터 함수 목록
	FORCEINLINE class UTexture2D* GetIcon() const { return ItemIcon; }
	FORCEINLINE const FString& GetSkillName() const { return SkillName; }
	FORCEINLINE const TArray<UAnimMontage*>& GetComboMontages() const { return ComboMontages; }
	FORCEINLINE EWeaponStance GetWeaponStance() const { return WeaponStance; }
	FORCEINLINE class UAnimMontage* GetSkillMontage() const { return SkillMontage; }
	FORCEINLINE float GetSkillDamage() const { return SkillDamage; }
	FORCEINLINE float GetSkillPoiseDamage() const { return SkillPoiseDamage; }
	FORCEINLINE class UParticleSystem* GetSkillVFX() const { return SkillVFX; }
	FORCEINLINE int32 GetSkillVFXSpawnCount() const { return SkillVFXSpawnCount; }
	FORCEINLINE float GetSkillTraceForwardOffset() const { return SkillTraceForwardOffset; }
	FORCEINLINE FVector GetSkillTraceBoxExtent() const { return SkillTraceBoxExtent; }
	FORCEINLINE float GetSkillTraceLength() const { return SkillTraceLength; }
};

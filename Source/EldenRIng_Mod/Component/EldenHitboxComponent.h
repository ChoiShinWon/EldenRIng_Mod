#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "EldenHitboxComponent.generated.h"

class UParticleSystem;
class USoundBase;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ELDENRING_MOD_API UEldenHitboxComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	UEldenHitboxComponent();

protected:
	virtual void BeginPlay() override;

	// 기존 Enemy에 있던 '방명록'
	UPROPERTY()
	TArray<AActor*> HitActors;

	// 기존 Enemy에 있던 오버랩 함수
	UFUNCTION()
	void OnHitboxOverlap(UPrimitiveComponent* OverlapComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 파티클 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Effect")
	UParticleSystem* HitVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Effect")
	UParticleSystem* GuardVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Effect")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Effect")
	USoundBase* GuardSound;


	void PlayImpactEffects(UParticleSystem* VFX, USoundBase* Sound, const FVector& Location) const;

public:
	//  데미지 량을 외부에서 설정할 수 있게 열어둠. (주먹은 10, 보스 무기는 50 등)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DamageAmount = 15.0f;

	// 기존 Enemy에 있던 콜리전 On/Off 스위치
	void EnableHitbox();
	void DisableHitbox();
};

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EldenPoiseComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPoiseBrokenDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDENRING_MOD_API UEldenPoiseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEldenPoiseComponent();

protected:

	virtual void BeginPlay() override;
	// 포이즈 = 체력처럼 깎이는 숨은 게이지
	// MaxPoise : 가득 찬 상태 = 균형 안정
	UPROPERTY(EditAnywhere, Category = "Poise")
	float MaxPoise = 50.0f;

	// CurrentPoise : 피격마다 ApplyPoiseDamage로 감소
	// 0 도달하면 균형 붕괴
	UPROPERTY(VisibleAnywhere, Category = "Poise")
	float CurrentPoise = 0.f;

	// 리젠
	float PoiseRegenRate = 20.0f; // 초당 회복량
	float PoiseRegenDelay = 3.f; // 마지막 피격 후 회복 시작까지 대기
	bool bCanRegen = true;
	FTimerHandle RegenDelayTimerHandle;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	void ApplyPoiseDamage(float PoiseDmg);


	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPoiseBrokenDelegate OnPoiseBroken;

private:
	void StartRegenDelay();
};

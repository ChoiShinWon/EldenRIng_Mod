
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EldenGameMode.generated.h"

class AEldenCharacter;
class AEldenGrace;
class ABloodstain;

UCLASS()
class ELDENRING_MOD_API AEldenGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AEldenGameMode();

	void RegisterGrace(AEldenGrace* Grace);

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	APlayerController* CachedPC = nullptr;

	UPROPERTY()
	AEldenCharacter* CachedPlayer = nullptr;

	UFUNCTION()
	void HandlePlayerDeath(AEldenCharacter* DeadPlayer);


	void RespawnPlayer();


	UPROPERTY()
	TWeakObjectPtr<AEldenGrace> LastGrace;


	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> YouDiedWidgetClass;

	// 스폰된 인스턴스 보관용 멤버
	UPROPERTY()
	UUserWidget* SpawnedWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Respawn")
	float RespawnDelay = 5.f;

	FTransform InitialSpawnTransform;

	FTimerHandle RespawnTimerHandle;

	// 마지막 방문 은총과 마찬가지로 위크 포인터 (관찰만, 소유 X)
	UPROPERTY()
	TWeakObjectPtr<ABloodstain> ActiveBloodstain;

	// BP_EldenGamMode에서 BP_Bloodstain 지정
	UPROPERTY(EditDefaultsOnly, Category = "Bloodstain")
	TSubclassOf<ABloodstain> BloodstainClass;

	void DropBloodstain(AEldenCharacter* DeadPlayer);
};

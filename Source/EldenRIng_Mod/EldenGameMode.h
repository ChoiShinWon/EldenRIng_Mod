// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EldenGameMode.generated.h"

class AEldenCharacter;
class AEldenGrace;

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
};

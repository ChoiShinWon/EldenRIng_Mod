#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "EldenSaveGame.generated.h"

class UEldenItemDefinition;
class UEldenTalismanDefinition;

// 스탯 묶음 구조체
USTRUCT()
struct FEldenSavedStats
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Level = 1;

	UPROPERTY()
	int32 Vigor = 10;

	UPROPERTY()
	int32 Mind = 10;

	UPROPERTY()
	int32 Endurance = 10;

	UPROPERTY()
	int32 Strength = 10;

	UPROPERTY()
	int32 CurrentRunes = 0;

	UPROPERTY()
	int32 RequiredRunes = 500;

};

USTRUCT()
struct FEldenSavedItem
{
	GENERATED_BODY()

	UPROPERTY()
	TSoftObjectPtr<UEldenItemDefinition> Item;

	UPROPERTY()
	int32 ItemCount = 0;
};

USTRUCT()
struct FEldenSavedInventory
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FEldenSavedItem> SavedItem;

	UPROPERTY()
	TSoftObjectPtr<UEldenItemDefinition> SelectedConsumable;

	UPROPERTY()
	TArray<TSoftObjectPtr<UEldenItemDefinition>> EquippedConsumables;

};

USTRUCT()
struct FEldenSavedEquipment
{
	GENERATED_BODY()

	// 방패 로테이션 (SpawnedShields)
	UPROPERTY()
	TArray<TSoftObjectPtr<UEldenItemDefinition>> LeftHand;

	// 무기 로테이션 (SpawnedWeapons)
	UPROPERTY()
	TArray<TSoftObjectPtr<UEldenItemDefinition>> RightHand;

	UPROPERTY()
	int32 LeftHandIndex = 0;

	UPROPERTY()
	int32 RightHandIndex = 0;

	UPROPERTY()
	TArray<TSoftObjectPtr<UEldenTalismanDefinition>> Talismans;
};

UCLASS()
class ELDENRING_MOD_API UEldenSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 SaveVersion = 1;

	UPROPERTY()
	FEldenSavedStats Stats;

	UPROPERTY()
	FName LastGraceId;

	UPROPERTY()
	FEldenSavedInventory SavedInventory;

	UPROPERTY()
	FEldenSavedEquipment SavedEquipment;

};

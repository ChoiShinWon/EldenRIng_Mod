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

	// 다음 레벨업에 필요한 룬. 레벨에서 재계산하지 않고 누적 상태값 그대로 저장
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

// 인벤토리 저장본. 장비 장착으로 이미 인벤토리에서 빠진 뒤의 상태
// 복원할 때 장비 쪽 (EquipmentComponent::ApplyFrom)에서 다시 차감하지 않음
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

USTRUCT()
struct FEldenSavedNPC
{
	GENERATED_BODY()

	// NPC 퀘스트 진행 단계
	UPROPERTY()
	int32 Stage = 0;

	UPROPERTY()
	int32 KillCount = 0;

	// NPC 위치값
	UPROPERTY()
	FTransform Transform;
};

USTRUCT()
struct FEldenSavedBloodstain
{
	GENERATED_BODY()

	// 룬 0 = 핏자국 없음
	UPROPERTY()
	int32 Runes = 0;

	UPROPERTY()
	FVector SpawnLocation = FVector::ZeroVector;
};

USTRUCT()
struct FEldenSavedWorld
{
	GENERATED_BODY()

	UPROPERTY()
	TSet<FName> PickupIDs;

	// TMap으로 관리하는 NPC별 상태 장부
	UPROPERTY()
	TMap<FName, FEldenSavedNPC> NPCs;

	UPROPERTY()
	FEldenSavedBloodstain Bloodstain;
};

// 세이브 1슬롯 분량의 데이터 컨테이너
// 호환 규칙: 필드 추가 -> 옛 파일은 기본값으로 읽혀서 안전 / 의미, 타입 변경 -> 마이그레이션 필요
UCLASS()
class ELDENRING_MOD_API UEldenSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FEldenSavedStats Stats;

	UPROPERTY()
	FName LastGraceId;

	UPROPERTY()
	FEldenSavedInventory SavedInventory;

	UPROPERTY()
	FEldenSavedEquipment SavedEquipment;

	UPROPERTY()
	FEldenSavedWorld SavedWorld;

};


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EldenGameMode.generated.h"

class AEldenCharacter;
class AEldenEnemy;
class AEldenGrace;
class ABloodstain;
class USoundBase;
class UAudioComponent;
class UEldenSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyKilledDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGraceRestedDelegate);

USTRUCT()
struct FEnemySpawnInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<AEldenEnemy> EnemyClass;

	UPROPERTY()
	FTransform SpawnTransform;

	UPROPERTY()
	TWeakObjectPtr<AEldenEnemy> Instance;

};

UCLASS()
class ELDENRING_MOD_API AEldenGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AEldenGameMode();

	void RegisterGrace(AEldenGrace* Grace);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEnemyKilledDelegate OnEnemyKilled;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGraceRestedDelegate OnGraceRested;

	void ResetAllEnemies();

	void HandleGraceRest(class AEldenGrace* Grace, class AEldenCharacter* Player);

	void SaveProgress();

	void ApplySavedProgress();

	// 레벨에 배치된 픽업만 호출할 것. GetFName()이 레벨 로드마다 같다는 전제로 ID로 씀.
	void MarkPickupCollected(FName ID);


protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	APlayerController* CachedPC = nullptr;

	UPROPERTY()
	AEldenCharacter* CachedPlayer = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* MainBGM = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio", meta = (ClampMin = "0.0"))
	float FadeInTime = 2.f;

	UPROPERTY()
	UAudioComponent* BGMComponent = nullptr;

	void StartBGM();

	UFUNCTION()
	void HandlePlayerDeath(AEldenCharacter* DeadPlayer);

	UFUNCTION()
	void HandleEnemyDied(int32 RuneReward, APawn* Killer);

	void BindEnemy(AEldenEnemy* Enemy);

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

	// 핏자국을 스폰하고 ActiveBloodstain에 등록하는 공용 헬퍼
	bool SpawnBloodstain(const FVector& Location, int32 Runes);

	UPROPERTY()
	TArray<FEnemySpawnInfo> EnemySpawnSnapshot;

private:
	TSet<FName> CollectedPickups;

	// 플레이어 상태 복원: 인벤토리 -> 장비 -> 스탯
	// 전제: CachedPlayer가 유효해야 함 (호출자 ApplySavedProgress가 가드)
	void RestorePlayerState(const UEldenSaveGame* Save);

	// 월드 플래그 복원: 픽업 / NPC / 핏자국
	void RestoreWorldState(const UEldenSaveGame* Save);

	// 마지막 은총을 찾아 등록하고 플레이어를 텔레포트
	// 전제: CachedPlayer가 유효해야 함 (호출자 ApplySavedProgress가 가드)
	void RestoreGracePosition(const UEldenSaveGame* Save);
};

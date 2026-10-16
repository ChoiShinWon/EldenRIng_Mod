#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EldenRing_Mod/Interface/Interactable.h"
#include "EldenNPC.generated.h"

class USphereComponent;
class USkeletalMeshComponent;
class UEldenDialogueData;
class AEldenCharacter;
class AEldenPlayerController;

// 다음 단계로 넘어가는 조건
UENUM(BlueprintType)
enum class ENPCStageAdvance : uint8
{
	None UMETA(ToolTip = "마지막 단계용. 안 넘어감"), 
	AfterTalk UMETA(ToolTip = "대사를 끝까지 보면 넘어감. RewardRunes는 이때만 지급"), 
	OnKillTarget UMETA(ToolTip = "KillTarget마리 처치하면 넘어감"), 
	OnGraceRest UMETA(ToolTip = "은총에서 쉬면 넘어감") 
};

USTRUCT(BlueprintType)
struct FNPCStage
{
	GENERATED_BODY()

	// 이 단계에서 말을 걸면 나오는 대사
	UPROPERTY(EditAnywhere, Category = "Stage")
	TObjectPtr<UEldenDialogueData> Dialogue;

	// 이 단계를 끝내는 조건
	UPROPERTY(EditAnywhere, Category = "Stage")
	ENPCStageAdvance Advance = ENPCStageAdvance::None;

	// Advance가 OnKillTarget일 때만 사용
	UPROPERTY(EditAnywhere, Category = "Stage")
	int32 KillTarget = 0;

	// 은총에서 쉰 뒤 이 단계가 현재 단계일 때 NPC가 서는 곳, 비우면 제자리 유지
	UPROPERTY(EditAnywhere, Category = "Stage")
	TObjectPtr<AActor> StandPoint = nullptr;

	// Advance가 AfterTalk일 때만 지급, 대화를 끝까지 본 순간 한 번
	UPROPERTY(EditAnywhere, Category = "Stage")
	int32 RewardRunes = 0;
};

UCLASS()
class ELDENRING_MOD_API AEldenNPC : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AEldenNPC();

	virtual void Interact(AEldenCharacter* Player) override;
	virtual FText GetInteractionPrompt() const override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* InteractionSphere = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* NPCMesh = nullptr;

	UPROPERTY(EditInstanceOnly, Category = "Dialogue")
	TArray<FNPCStage> Stages;

private:
	int32 CurrentLineIndex = 0;
	bool bTalking = false;
	TWeakObjectPtr<AEldenCharacter> CurrentPlayer;

	int32 CurrentStage = 0;

	int32 KillCount = 0;

	UFUNCTION()
	void HandleEnemyKilled();

	UFUNCTION()
	void HandleGraceRested();

	UPROPERTY()
	TObjectPtr<UEldenDialogueData> CurrentDialogue;

	UEldenDialogueData* PickDialogue() const;

	void AdvanceStage();

	void StartDialogue(AEldenCharacter* Player);
	void AdvanceDialogue();
	void EndDialogue();

	AEldenPlayerController* GetCurrentPC() const;

};

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
	None, // 마지막 단계: 더 이상 안넘어감
	AfterTalk, // 대화가 끝나면
	OnKillTarget, // 적을 KillTarget 마리 처치하면
	OnGraceRest // 은총에서 쉬면
};

USTRUCT(BlueprintType)
struct FNPCStage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Stage")
	TObjectPtr<UEldenDialogueData> Dialogue;

	UPROPERTY(EditAnywhere, Category = "Stage")
	ENPCStageAdvance Advance = ENPCStageAdvance::None;

	UPROPERTY(EditAnywhere, Category = "Stage")
	int32 KillTarget = 0;

	UPROPERTY(EditAnywhere, Category = "Stage")
	TObjectPtr<AActor> StandPoint = nullptr;

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

	UPROPERTY()
	TObjectPtr<UEldenDialogueData> CurrentDialogue;

	UEldenDialogueData* PickDialogue() const;

	void AdvanceStage();

	void StartDialogue(AEldenCharacter* Player);
	void AdvanceDialogue();
	void EndDialogue();

	AEldenPlayerController* GetCurrentPC() const;

};

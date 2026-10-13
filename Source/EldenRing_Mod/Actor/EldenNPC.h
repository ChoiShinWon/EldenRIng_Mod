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

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TObjectPtr<UEldenDialogueData> DialogueData;

private:
	int32 CurrentLineIndex = 0;
	bool bTalking = false;
	TWeakObjectPtr<AEldenCharacter> CurrentPlayer;

	void StartDialogue(AEldenCharacter* Player);
	void AdvanceDialogue();
	void EndDialogue();

	AEldenPlayerController* GetCurrentPC() const;

};

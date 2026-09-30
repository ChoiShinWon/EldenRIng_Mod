#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EldenRing_Mod/Interface/Interactable.h"
#include "EldenItemPickUp.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UEldenItemDefinition;
class AEldenCharacter;

UCLASS()
class ELDENRING_MOD_API AEldenItemPickUp : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AEldenItemPickUp();

	virtual void Interact(AEldenCharacter* Player) override;
	virtual FText GetInteractionPrompt() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* InteractionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UEldenItemDefinition* ItemDef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PickAmount = 1;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};

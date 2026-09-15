
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EldenRing_Mod/Interface/Interactable.h"
#include "Bloodstain.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UParticleSystem;
class AEldenCharacter;

UCLASS()
class ELDENRING_MOD_API ABloodstain : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	ABloodstain();

	// 손실 룬을 주입하는 진입점
	void InitBloodstain(int32 Amount);

	virtual void Interact(AEldenCharacter* Player) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* InteractionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BloodstainMesh;

	UPROPERTY(EditAnywhere)
	UParticleSystem* PickupVFX;


	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	// 회수 시 플레이어에게 되돌려줄 값
	int32 StoredRunes = 0;
};

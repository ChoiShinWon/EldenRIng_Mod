#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EldenAreaTrigger.generated.h"

class UBoxComponent;

UCLASS()
class ELDENRING_MOD_API AEldenAreaTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	AEldenAreaTrigger();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Trigger")
	UBoxComponent* TriggerBox = nullptr;

	UPROPERTY(EditAnywhere, Category = "Trigger")
	FText AreaName;

	UPROPERTY(EditAnywhere, Category = "Trigger")
	float ShowDelay;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);
};

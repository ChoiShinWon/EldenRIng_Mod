
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EldenGraceRestComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDENRING_MOD_API UEldenGraceRestComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEldenGraceRestComponent();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grace")
	class UAnimMontage* SitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grace")
	class UAnimMontage* StandUpMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grace")
	float ExitCameraBlendTime = 0.5f;

	void EnterRest();
	void ExitRest();

	UFUNCTION()
	void OnStandUpMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	UPROPERTY()
	class AEldenCharacter* OwnerCharacter;
};

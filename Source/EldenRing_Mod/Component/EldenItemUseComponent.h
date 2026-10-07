
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EldenItemUseComponent.generated.h"

class AEldenCharacter;
class UAnimMontage;
class UEldenInventoryComponent;
class UEldenStatComponent;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDENRING_MOD_API UEldenItemUseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEldenItemUseComponent();

	void UseItem();

	void SwitchItem();

	void ApplyItemEffect();

protected:
	virtual void BeginPlay() override;

private:
	void StartDrinkingPotion();
	void OnPotionMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY()
	AEldenCharacter* OwnerCharacter = nullptr;

	UPROPERTY()
	UEldenInventoryComponent* CachedInventoryComp = nullptr;

	UPROPERTY()
	UEldenStatComponent* CachedStatComp = nullptr;
};

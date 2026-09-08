
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELDENRING_MOD_API IInteractable
{
	GENERATED_BODY()

public:
	virtual void Interact(class AEldenCharacter* Player) = 0;
};

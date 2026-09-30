#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EldenRing_Mod/Interface/Interactable.h"
#include "EldenInteractionComponent.generated.h"

class AEldenCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ELDENRING_MOD_API UEldenInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEldenInteractionComponent();

protected:
	virtual void BeginPlay() override;

	// CurrentInteractableTarget을 여기로 옮기기
	TScriptInterface<class IInteractable> CurrentInteractableTarget;

public:
	void SetInteractableTarget(TScriptInterface<class IInteractable> NewTarget);

	void ExecuteInteract();

private:
	// 소유자 캐릭터 캐싱
	UPROPERTY()
	class AEldenCharacter* PlayerCharacter;
};

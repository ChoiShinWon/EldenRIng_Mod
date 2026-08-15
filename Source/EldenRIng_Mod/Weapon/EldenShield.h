
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EldenShield.generated.h"

class UEldenHitboxComponent;

UCLASS()
class ELDENRING_MOD_API AEldenShield : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEldenShield();

	void EnableShieldBlock();
	void DisableShieldBlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, Category = "Shield")
	class UStaticMeshComponent* ShieldMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shield")
	UEldenHitboxComponent* ShieldHitbox;

};

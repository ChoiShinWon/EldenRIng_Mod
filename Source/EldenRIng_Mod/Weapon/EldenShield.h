
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

	UPROPERTY(EditAnywhere, Category = "UI")
	class UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, Category = "UI")
	FString SkillName;


	UPROPERTY(VisibleAnywhere, Category = "Shield")
	class UStaticMeshComponent* ShieldMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shield")
	UEldenHitboxComponent* ShieldHitbox;

public:
	FORCEINLINE class UTexture2D* GetIcon() const { return ItemIcon; }
	FORCEINLINE const FString& GetSkillName() const { return SkillName; }

};


#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EldenItemDefinition.generated.h"

class UTexture2D;
class UAnimMontage;


UENUM(BlueprintType)
enum class EItemType : uint8
{
	None UMETA(DisplayName = "None"),
	HP_Potion UMETA(DisplayName = "HPPotion"),
	Mana_Potion UMETA(DisplayName = "ManaPotion"),
	GoldenRune UMETA(DisplayName = "GodlenRune")
};

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	None UMETA(DisplayName = "None"),
	Usable UMETA(DisplayName = "Usable"),
	Weapon UMETA(DisplayName = "Weapon"),
	Armor UMETA(DisplayName = "Armor"),
	Key UMETA(DisplayName = "Key")
};


UCLASS(BlueprintType)
class ELDENRING_MOD_API UEldenItemDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	EItemType ItemType = EItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UAnimMontage> UseMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	int32 MaxCount = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Potion")
	float HealAmount = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Potion")
	FLinearColor DrinkGlowColor = FLinearColor::Red;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rune")
	int32 RuneAmount = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	EItemCategory Category = EItemCategory::None;
};

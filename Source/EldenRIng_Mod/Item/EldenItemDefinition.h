// Fill out your copyright notice in the Description page of Project Settings.

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
	HP_Potion UMETA(DisplayName = "HPPotion")
	// 나중에 마나 물약이나 사용 아이템 등을 여기에 추가
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float HealAmount = 30.0f;
};

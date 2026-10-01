
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EldenItemDefinition.generated.h"

class UTexture2D;
class UAnimMontage;

// 소모품(사용 가능한 아이템) 한정 세부 종류
UENUM(BlueprintType)
enum class EConsumableType : uint8
{
	None UMETA(DisplayName = "None"),
	HP_Potion UMETA(DisplayName = "HPPotion"),
	Mana_Potion UMETA(DisplayName = "ManaPotion"),
	GoldenRune UMETA(DisplayName = "GodlenRune")
};

// 아이템 전부를 아우르는 대분류
UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	None UMETA(DisplayName = "None"),
	Usable UMETA(DisplayName = "Usable"),
	Weapon UMETA(DisplayName = "Weapon"),
	Armor UMETA(DisplayName = "Armor"),
	Key UMETA(DisplayName = "Key")
};

// 월드에 스폰되는 액터가 아니라 콘텐츠 브라우저에 저장해두고 재사용하는 순수 데이터 에셋
// 모든 아이템 종류의 필드를 한 클래스에 다 들고 있음
// 서브클래스 분리 고려
UCLASS(BlueprintType)
class ELDENRING_MOD_API UEldenItemDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// 무기/방패 아이템일 때만 채워짐. 인벤토리에 있는 동안은 참조 안됨.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AEldenWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Shield")
	TSubclassOf<class AEldenShield> ShieldClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	EConsumableType ItemType = EConsumableType::None;

	// UI 표시용 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	// UI 표시용 아이콘
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UTexture2D> Icon = nullptr;

	// 소모품 사용시 재생할 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UAnimMontage> UseMontage = nullptr;

	// 인벤토리에서 이 아이템 하나가 쌓일 수 있는 최대 스택 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	int32 MaxCount = 3;

	// 포션 전용 필드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Potion")
	float HealAmount = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Potion")
	FLinearColor DrinkGlowColor = FLinearColor::Red;

	// 황금룬 전용 필드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rune")
	int32 RuneAmount = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	EItemCategory Category = EItemCategory::None;
};

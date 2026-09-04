
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EldenRing_Mod/Item/EldenItemDefinition.h"
#include "EldenInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPotionCountChangeDelegate, int32, CurrentValue, int32, MaxValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectedItemChangedDelegate);

USTRUCT(BlueprintType) // BP에서 슬롯 내용 읽을 수 있게
struct FEldenItemSlot
{
	GENERATED_BODY()

	// 어떤 아이템인가
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UEldenItemDefinition> Definition = nullptr;

	// 지금 몇개 남았나
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	int32 Count = 0;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDENRING_MOD_API UEldenInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEldenInventoryComponent();

protected:
	
	virtual void BeginPlay() override;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TArray<FEldenItemSlot> ItemSlots;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	int32 SelectedIndex = 0;

public:	
	// 캐릭터가 지금 무슨 아이템 들고있냐고 물어볼 때 대답해줄 Getter 함수
	EItemType GetCurrentSelectedItem() const 
	{ 
		const UEldenItemDefinition* Def = GetSelectedDefinition();
		return Def ? Def->ItemType : EItemType::None;
	}


	UAnimMontage* GetCurrentUseMontage() const 
	{ 
		const UEldenItemDefinition* Def = GetSelectedDefinition();
		return Def ? Def->UseMontage : nullptr;
	}

	// 아이템 사용 가능한지 확인하는 함수
	bool CanUseItem() const;

	// 실제로 포션을 하나 소모할 함수
	void ConsumeItem();

	// 외부(캐릭터)에서 포션의 회복량을 가져갈 수 있게 해주는 게터 함수
	float GetPotionHealAmount() const;

	// 포션 리필 함수
	void RefillPotions();

	// 현재 선택된 아이템의 아이콘을 돌려주는 Getter 함수
	UTexture2D* GetCurrentItemIcon() const;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSelectedItemChangedDelegate OnSelectedItemChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPotionCountChangeDelegate OnPotionCountChanged;


	int32 GetCurrentPotionCount() const 
	{ 
		const FEldenItemSlot* Slot = GetSelectedSlot();
		return Slot ? Slot->Count : 0;
	}

	int32 GetMaxPotionCount() const 
	{ 
		const UEldenItemDefinition* Def = GetSelectedDefinition();
		return Def ? Def->MaxCount : 0; 
	}
	
	void SelectNextItem();
	
private:
	void BroadcastPotionCount();
	void SetSelectedIndex(int32 NewIndex);

	// 선택된 슬롯을 안전하게 꺼내는 함수. 빈 배열이거나 인덱스 범위 밖이면 nullptr
	const FEldenItemSlot* GetSelectedSlot() const;

	FEldenItemSlot* GetSelectedSlot();

	const UEldenItemDefinition* GetSelectedDefinition() const;
};

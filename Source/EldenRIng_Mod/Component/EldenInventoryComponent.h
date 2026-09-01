// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EldenRing_Mod/Item/EldenItemDefinition.h"
#include "EldenInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPotionCountChangeDelegate, int32, CurrentValue, int32, MaxValue);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDENRING_MOD_API UEldenInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEldenInventoryComponent();

protected:
	
	virtual void BeginPlay() override;
 
	// 현재 남은 포션 개수
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Potion")
	int32 CurrentPotionCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TObjectPtr<UEldenItemDefinition> EquippedItem;
	

public:	
	// 캐릭터가 지금 무슨 아이템 들고있냐고 물어볼 때 대답해줄 Getter 함수
	EItemType GetCurrentSelectedItem() const { return EquippedItem ? EquippedItem->ItemType : EItemType::None; }


	UAnimMontage* GetCurrentUseMontage() const { return EquippedItem ? EquippedItem->UseMontage : nullptr; }

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
	FOnPotionCountChangeDelegate OnPotionCountChanged;

	FORCEINLINE int32 GetCurrentPotionCount() const { return CurrentPotionCount; }
	FORCEINLINE int32 GetMaxPotionCount() const { return EquippedItem ? EquippedItem->MaxCount : 0; }
	
private:
	void BroadcastPotionCount();
};

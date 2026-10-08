
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EldenRing_Mod/Item/EldenItemDefinition.h"
#include "EldenInventoryComponent.generated.h"

// HUD가 포션 개수(현재/최대)를 갱신할 때 구독하는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPotionCountChangeDelegate, int32, CurrentValue, int32, MaxValue);
// 핫바에서 선택한 아이템이 바뀔 때마다 구독자(HUD 등)에게 알리는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectedItemChangedDelegate);

// 인벤토리 한 칸의 실제 데이터 (어떤 아이템 + 몇 개) - TMap의 Value로 쓰임
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
	// HP/마나 포션 자동 지급 + 이전에 선택했던 소모품 복구/기본값 세팅 + 포션 최대치로 채우기
	virtual void BeginPlay() override;
 

	// 해시 기반 보유량 조회용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TMap<TObjectPtr<UEldenItemDefinition>, FEldenItemSlot> ItemMap;

	// TMap은 순회 순서를 보장 안 하므로
	// UI 핫바에서 습득한 순서대로 보여주기 위해 따로 순서만 저장하는 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TArray<TObjectPtr<UEldenItemDefinition>> ItemOrder;

	// 지금 핫바에 선택된 소모품 정의
	UPROPERTY(VisibleAnywhere, Category = "Item")
	TObjectPtr<UEldenItemDefinition> SelectedConsumable = nullptr;

	// 일반 소모품(HP/마나 포션 제외)이 핫바 로테이션에 들어갈 수 있는 최대 정원
	UPROPERTY(EditAnywhere, Category = "Item")
	int32 MaxEquippedSlots = 10;

	// 핫바 로테이션에 장착된 일반 소모품 목록
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	TArray<TObjectPtr<UEldenItemDefinition>> EquippedConsumables;

	// HP 포션 정의 - 습득 대상이 아니라 BeginPlay에서 자동 지급되는 고정 슬롯
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TObjectPtr<UEldenItemDefinition> HPPotionDef;

	// 마나 포션 정의 - HP포션과 동일하게 고정 슬롯 취급
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TObjectPtr<UEldenItemDefinition> ManaPotionDef;

public:
	// 일반 소모품을 핫바에 추가 (Usable 카테고리만 허용)
	UFUNCTION(BlueprintCallable, Category = "Item")
	bool EquipConsumable(UEldenItemDefinition* Definition);

	// 일반 소모품을 핫바에서 제거
	UFUNCTION(BlueprintCallable, Category = "Item")
	bool UnequipConsumable(UEldenItemDefinition* Definition);



	// 캐릭터가 지금 무슨 아이템 들고있냐고 물어볼 때 대답해줄 Getter 함수
	EConsumableType GetCurrentSelectedItem() const
	{
		const UEldenItemDefinition* Def = GetSelectedDefinition();
		return Def ? Def->ItemType : EConsumableType::None;
	}

	// 지금 선택된 아이템을 사용할 때 재생할 몽타주
	UAnimMontage* GetCurrentUseMontage() const 
	{ 
		const UEldenItemDefinition* Def = GetSelectedDefinition();
		return Def ? Def->UseMontage : nullptr;
	}

	// 아이템 습득 - 이미 갖고 있으면 개수만 증가, 처음이면 ItemMap + ItemOrder에 새로 등록
	bool AddItem(UEldenItemDefinition* Definition, int32 Amount);

	// 아이템 소모/제거 - 개수가 0이 되면 ItemMap, ItemOrder에서 삭제
	bool RemoveItem(UEldenItemDefinition* Definition, int32 Amount);

	// 특정 아이템을 몇 개 가지고 있는 조회 (O(1) 해시 조회)
	int32 GetItemCount(const UEldenItemDefinition* Definition) const;

	// 특정 아이템을 1개 이상 갖고 있는지 여부
	bool HasItem(const UEldenItemDefinition* Definition) const;

	// 아이템 사용 가능한지 확인하는 함수
	bool CanUseItem() const;

	// 실제로 포션을 하나 소모할 함수
	void ConsumeItem();

	// 외부(캐릭터)에서 포션의 회복량을 가져갈 수 있게 해주는 게터 함수
	float GetPotionRestoreAmount() const;

	// 포션 리필 함수 (은총 휴식등에서 최대치로 되돌릴 때 사용)
	void RefillPotions();

	// 현재 선택된 아이템의 아이콘을 돌려주는 Getter 함수
	UTexture2D* GetCurrentItemIcon() const;

	// 현재 선택된 아이템(포션)을 마실 때의 발광 색상
	FLinearColor GetCurrentDrinkGlowColor() const;

	// ItemOrder를 읽기 전용으로 가져올 Getter 함수
	const TArray <TObjectPtr<UEldenItemDefinition>>& GetItemOrder() const;

	// 일반 소모품 핫바 로테이션의 최대 정원
	int32 GetMaxEquippedSlots() const;

	// 핫바 선택 변경 시 브로드캐스트 (HUD 등이 구독)
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSelectedItemChangedDelegate OnSelectedItemChanged;

	// 포션 개수 변경 시 브로드캐스트 (HUD 게이지 등이 구독)
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPotionCountChangeDelegate OnPotionCountChanged;

	// 현재 슬롯에 있는 아이템의 현재 개수
	int32 GetCurrentItemCount() const
	{
		const FEldenItemSlot* Slot = GetSelectedSlot();
		return Slot ? Slot->Count : 0;
	}

	// 현재 슬롯에 있는 아이템의 최대 개수
	int32 GetMaxItemCount() const
	{
		const UEldenItemDefinition* Def = GetSelectedDefinition();
		return Def ? Def->MaxCount : 0;
	}

	// 핫바 다음 아이템으로 순환 선택(HP 포션 -> 마나 포션 -> 소모품...)
	void SelectNextItem();

	// 핫바에 장착된 일반 소모품 목록 조회 (장비창 UI가 이 배열 그대로 그림)
	const TArray<TObjectPtr<UEldenItemDefinition>>& GetEquippedConsumables() const;
	
private:
	// OnPotionCountChanged 델리게이트에 현재/최대 개수를 실어 브로드캐스트 하는 공용 헬퍼
	void BroadcastPotionCount();

	// 선택된 소모품을 바꾸고, 관련 델리게이트 두 개를 함께 브로드캐스트
	void SetSelectedConsumable(UEldenItemDefinition* Selected);

	// 지금 선택된 아이템의 슬롯 가져오기 (읽기 전용)
	const FEldenItemSlot* GetSelectedSlot() const;

	// 지금 선택된 아이템 슬롯 찾아옴 (Count를 직접 바꿔야 할 때 사용)
	FEldenItemSlot* GetSelectedSlot();

	// 지금 선택된 아이템의 정의를 반환
	const UEldenItemDefinition* GetSelectedDefinition() const;

	// 핫바 순환 순서 조립: HP포션 -> 마나 포션 -> 장착된 일반 소모품
	TArray<UEldenItemDefinition*> BuildCycleOrder() const;
};

#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "Engine/Texture.h"

UEldenInventoryComponent::UEldenInventoryComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;

	
}


void UEldenInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// HP/마나 포션은 줍는 아이템이 아니라 시작하자마자 무조건 최대치로 지급되는 고정 슬롯
	// 이미 갖고 있다면 또 지급하지 않게 HasItem으로 방어
	if (HPPotionDef && !HasItem(HPPotionDef)) AddItem(HPPotionDef, HPPotionDef->MaxCount);
	if (ManaPotionDef && !HasItem(ManaPotionDef)) AddItem(ManaPotionDef, ManaPotionDef->MaxCount);
	
	
	TArray<UEldenItemDefinition*> CycleOrder = BuildCycleOrder();

	// 저장된 SelectedConsumable이 여전히 유효환 순환 목록 안에 있다면 유지
	if (SelectedConsumable && CycleOrder.Contains(SelectedConsumable))
	{

	}
	else
	{
		// 유효하지 않다면 순환 목록의 첫 번째로, 그마저 없으면 선택 없음.
		if (CycleOrder.Num() >= 1) SelectedConsumable = CycleOrder[0];
		else SelectedConsumable = nullptr;
	}

	// 시작시 HP/마나 포션을 항상 최대치로 채움
	RefillPotions();
}

TArray<UEldenItemDefinition*> UEldenInventoryComponent::BuildCycleOrder() const
{
	// HP 포션-> 마나 포션-> 장착된 소모품들, 이 순서 고정
	TArray<UEldenItemDefinition*> CycleOrder;
	// HP 포션 먼저
	if (HPPotionDef) CycleOrder.Add(HPPotionDef);
	// 다음 마나 포션
	if (ManaPotionDef) CycleOrder.Add(ManaPotionDef);
	// 그 외 소모품들 Append로 연결
	CycleOrder.Append(EquippedConsumables);

	return CycleOrder;
}

bool UEldenInventoryComponent::EquipConsumable(UEldenItemDefinition* Definition)
{
	// HP, 마나 포션은 이미 고정이라 장착/해제라는 개념 자체가 없음
	if (Definition == HPPotionDef || Definition == ManaPotionDef) return false;
	if (!Definition) return false;

	// 소모품 카테고리만 장착 하용
	if (Definition->Category == EItemCategory::Usable)
	{
		// 인벤토리에 보유중인가
		if (HasItem(Definition))
		{
			// 이미 장착되어있다면 중복 금지
			if (EquippedConsumables.Contains(Definition)) return false;

			// 정원이 꽉 찼다면 못넣음
			if (MaxEquippedSlots == EquippedConsumables.Num()) return false;

			// 조건을 통과한다면 장착 목록에 추가
			EquippedConsumables.Add(Definition);
			return true;
			
		}
	}

	return false;
}

bool UEldenInventoryComponent::UnequipConsumable(UEldenItemDefinition* Definition)
{
	// HP/마나 포션은 해제 대상이 아님
	if (Definition == HPPotionDef || Definition == ManaPotionDef) return false;

	if (!Definition) return false;
	if (Definition->Category == EItemCategory::Usable)
	{
		int32 RemovedCount = EquippedConsumables.Remove(Definition);
		// 앞에서 Contains 체크로 중복을 막아놨으니 값은 0 아니면 1이 return된다
		// Remove값은 이 값과 같은 원소를 배열에서 찾아 지우고, 몇 개 지웠는지 리턴한다
		return RemovedCount > 0; // 하나라도 지웠으면 성공
	}
	return false;
}

bool UEldenInventoryComponent::AddItem(UEldenItemDefinition* Definition, int32 Amount)
{
	if (!Definition || Amount <=0 ) return false;
	FEldenItemSlot* FoundSlot = ItemMap.Find(Definition);
	// Find는 그 키가 있으면 Value(FEldenItemSlot)의 포인터, 없으면 nullptr 리턴

	if (FoundSlot) // 이미 그 아이템을 갖고 있는 슬롯이 있다면 -> 개수만 늘린다.
	{
		// MaxCount를 넘지 않도록 클램프
		FoundSlot->Count = FMath::Clamp(FoundSlot->Count + Amount, 0, FoundSlot->Definition->MaxCount);
	}
	else // 인벤토리 슬롯에 없다 -> 새 슬롯 만든다
	{
		FEldenItemSlot NewSlot; // 구조체기 때문에 스택에 값 자체 생성
		NewSlot.Definition = Definition;
		NewSlot.Count = FMath::Clamp(Amount, 0, Definition->MaxCount);
		ItemMap.Add(Definition, NewSlot); // 해시맵에 키-값 쌍으로 등록
		ItemOrder.Add(Definition); // 순서 배열에도 추가
	}
	return true;
}

bool UEldenInventoryComponent::RemoveItem(UEldenItemDefinition* Definition, int32 Amount)
{
	if (!Definition || Amount <= 0) return false;
	FEldenItemSlot* FoundSlot = ItemMap.Find(Definition);
	if (!FoundSlot) return false; // 애초에 갖고 있지 않은 아이템은 뺄 수 없다

	FoundSlot->Count = FMath::Clamp(FoundSlot->Count - Amount, 0, FoundSlot->Definition->MaxCount);
	if (FoundSlot->Count == 0)
	{
		// 0개가 되면 슬롯 자체를 제거 - ItmeMap/ItemOrder를 둘 다 지워야 일관됨
		ItemMap.Remove(Definition);
		ItemOrder.Remove(Definition);
	}

	return true;
}

int32 UEldenInventoryComponent::GetItemCount(const UEldenItemDefinition* Definition) const
{
	// const 메서드 안에서 Find를 부르면 리턴도 const 포인터가 된다
	const FEldenItemSlot* FoundSlot = ItemMap.Find(Definition);
	
	return FoundSlot ? FoundSlot->Count : 0; // 없다면 0개로 취급
}

bool UEldenInventoryComponent::HasItem(const UEldenItemDefinition* Definition) const
{
	return GetItemCount(Definition) > 0;
}

bool UEldenInventoryComponent::CanUseItem() const
{
	const FEldenItemSlot* Slot = GetSelectedSlot();
	return Slot && Slot->Count > 0;
}

void UEldenInventoryComponent::ConsumeItem()
{
	if (!CanUseItem()) return;
	GetSelectedSlot()->Count--;
	BroadcastPotionCount();
}

float UEldenInventoryComponent::GetPotionRestoreAmount() const
{
	const UEldenItemDefinition* Def = GetSelectedDefinition();
	return Def ? Def->HealAmount : 0.0f;
}

void UEldenInventoryComponent::RefillPotions()
{
	// 이미 알고 있는 키 (HPPotionDef/ManaPotionDef)로 직접 조회 O(1)
	if (FEldenItemSlot* HPSlot = ItemMap.Find(HPPotionDef))
	{
		HPSlot->Count = HPPotionDef->MaxCount;
	}
	if (FEldenItemSlot* ManaSlot = ItemMap.Find(ManaPotionDef))
	{
		ManaSlot->Count = ManaPotionDef->MaxCount;
	}
	
	BroadcastPotionCount();
}

UTexture2D* UEldenInventoryComponent::GetCurrentItemIcon() const
{
	const UEldenItemDefinition* Def = GetSelectedDefinition();
	return Def ? Def->Icon : nullptr;
}

FLinearColor UEldenInventoryComponent::GetCurrentDrinkGlowColor() const
{
	const UEldenItemDefinition* Def = GetSelectedDefinition();
	return Def ? Def->DrinkGlowColor : FLinearColor::White;
}

const TArray<TObjectPtr<UEldenItemDefinition>>& UEldenInventoryComponent::GetItemOrder() const
{
	return ItemOrder;
}

int32 UEldenInventoryComponent::GetMaxEquippedSlots() const
{
	return MaxEquippedSlots;
}

void UEldenInventoryComponent::SetSelectedConsumable(UEldenItemDefinition* Selected)
{
	SelectedConsumable = Selected;
	// 선택이 바뀌었다는 것과, 그로 인해 포션 개수 표시도 바뀌었다는 것을 둘 다 알림
	OnSelectedItemChanged.Broadcast();
	BroadcastPotionCount();
}

void UEldenInventoryComponent::SelectNextItem()
{
	TArray<UEldenItemDefinition*> CycleOrder = BuildCycleOrder();
	
	// 아이템 슬롯이 0개나 1개면 바꿀 게 없음 -> 그냥 return
	if (CycleOrder.Num() <= 1) return;
	int32 NextIndex = 0;

	int32 CurrentIndex = CycleOrder.Find(SelectedConsumable);
	if (CurrentIndex == INDEX_NONE)
	{
		// 현재 선택한 게 순환 목록에 없다면 첫 번째로 리셋
		NextIndex = 0;
	}
	else
	{
		// 마지막 다음은 다시 처음으로
		NextIndex = (CurrentIndex + 1) % CycleOrder.Num();
	}

	SetSelectedConsumable(CycleOrder[NextIndex]);
}

const TArray<TObjectPtr<UEldenItemDefinition>>& UEldenInventoryComponent::GetEquippedConsumables() const
{
	return EquippedConsumables;
}


void UEldenInventoryComponent::BroadcastPotionCount()
{
	OnPotionCountChanged.Broadcast(GetCurrentItemCount(), GetMaxItemCount());
}

const FEldenItemSlot* UEldenInventoryComponent::GetSelectedSlot() const
{
	return ItemMap.Find(SelectedConsumable);
}

FEldenItemSlot* UEldenInventoryComponent::GetSelectedSlot()
{
	return ItemMap.Find(SelectedConsumable);
}

const UEldenItemDefinition* UEldenInventoryComponent::GetSelectedDefinition() const
{
	return SelectedConsumable;
}

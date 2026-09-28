#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "Engine/Texture.h"

UEldenInventoryComponent::UEldenInventoryComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;

	
}


void UEldenInventoryComponent::BeginPlay()
{
	Super::BeginPlay();


	if (HPPotionDef && !HasItem(HPPotionDef)) AddItem(HPPotionDef, HPPotionDef->MaxCount);
	if (ManaPotionDef && !HasItem(ManaPotionDef)) AddItem(ManaPotionDef, ManaPotionDef->MaxCount);
	
	
	TArray<UEldenItemDefinition*> CycleOrder = BuildCycleOrder();
	
	if (SelectedConsumable && CycleOrder.Contains(SelectedConsumable))
	{

	}
	else
	{
		if (CycleOrder.Num() >= 1) SelectedConsumable = CycleOrder[0];
		else SelectedConsumable = nullptr;
	}
		
	RefillPotions();
}

TArray<UEldenItemDefinition*> UEldenInventoryComponent::BuildCycleOrder() const
{
	TArray<UEldenItemDefinition*> CycleOrder;
	if (HPPotionDef) CycleOrder.Add(HPPotionDef);
	if (ManaPotionDef) CycleOrder.Add(ManaPotionDef);
	CycleOrder.Append(EquippedConsumables);

	return CycleOrder;
}

bool UEldenInventoryComponent::EquipConsumable(UEldenItemDefinition* Definition)
{
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
	if (Definition == HPPotionDef || Definition == ManaPotionDef) return false;

	if (!Definition) return false;
	if (Definition->Category == EItemCategory::Usable)
	{
		int32 RemovedCount = EquippedConsumables.Remove(Definition);
		// 앞에서 Contains 체크로 중을 막아놨으니 값은 0 아니면 1이 return된다
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
	for (auto& Pair : ItemMap)
	{
		if (Pair.Value.Definition->ItemType == EItemType::HP_Potion ||
			Pair.Value.Definition->ItemType == EItemType::Mana_Potion)
		{
			Pair.Value.Count = Pair.Value.Definition->MaxCount;
		}
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

void UEldenInventoryComponent::SetSelectedConsumable(UEldenItemDefinition* Selected)
{
	SelectedConsumable = Selected;
	OnSelectedItemChanged.Broadcast();
	BroadcastPotionCount();
}

void UEldenInventoryComponent::SelectNextItem()
{
	TArray<UEldenItemDefinition*> CycleOrder = BuildCycleOrder();
	
	// 아이템 슬롯이 0개나 1개면 바꿀 게 없음 -> 그냥 return
	if (CycleOrder.Num() <= 1) return;
	int32 NextIndex;

	int32 CurrentIndex = CycleOrder.Find(SelectedConsumable);
	if (CurrentIndex == INDEX_NONE)
	{
		NextIndex = 0;
	}
	else
	{
		NextIndex = (CurrentIndex + 1) % CycleOrder.Num();
	}

	SetSelectedConsumable(CycleOrder[NextIndex]);
}


void UEldenInventoryComponent::BroadcastPotionCount()
{
	OnPotionCountChanged.Broadcast(GetCurrentPotionCount(), GetMaxPotionCount());
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

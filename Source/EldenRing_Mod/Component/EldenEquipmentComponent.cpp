#include "EldenRing_Mod/Component/EldenEquipmentComponent.h"
#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Item/EldenItemDefinition.h"
#include "EldenRing_Mod/Item/EldenTalismanDefinition.h"
#include "EldenRing_Mod/Weapon/EldenWeapon.h"
#include "EldenRing_Mod/Weapon/EldenShield.h"

UEldenEquipmentComponent::UEldenEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UEldenEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<AEldenCharacter>(GetOwner());
	if (!PlayerCharacter) return;
	// 기본/무기 방패를 로테이션에 추가 (자동 활성화는 안 되므로 아래에서 직접 활성화)
	EquipWeapon(DefaultWeaponDef);
	EquipShield(DefaultShieldDef);

	// 방금 추가한 인덱스 0을 명시적으로 활성화 (nullptr 방어 후 역참조)
	EquippedWeapon = SpawnedWeapons.IsValidIndex(0) ? SpawnedWeapons[0] : nullptr;
	if (EquippedWeapon) EquippedWeapon->SetActorHiddenInGame(false);
	EquippedShield = SpawnedShields.IsValidIndex(0) ? SpawnedShields[0] : nullptr;
	if (EquippedShield) EquippedShield->SetActorHiddenInGame(false);
	
	for (UEldenTalismanDefinition* Def : DefaultTalismans)
	{
		EquipTalisman(Def);
	}
}


bool UEldenEquipmentComponent::EquipWeapon(UEldenItemDefinition* Item)
{
	// 무기 아이템이 아니면 (WeaponClass가 없다면) 리턴
	if (!Item || !Item->WeaponClass) return false;

	// 다섯개가 꽉 차있다면 반환
	if (SpawnedWeapons.Num() >= 5) return false;


	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = PlayerCharacter;
	SpawnParams.Instigator = PlayerCharacter->GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 인벤토리(데이터)에 있던 아이템을 실제 월드 액터로 스폰 - 장착 시에만 액터 생성
	AEldenWeapon* NewWeapon = GetWorld()->SpawnActor<AEldenWeapon>(Item->WeaponClass,
		PlayerCharacter->GetActorLocation(), PlayerCharacter->GetActorRotation(), SpawnParams);
	if (!NewWeapon) return false;

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	NewWeapon->AttachToComponent(PlayerCharacter->GetMesh(), AttachmentRules, FName("RightHandSocket"));

	// 새로 장착해도 자동으로 활상화 하진 않음, 로테이션에 추가해놓고 Visible은 꺼둠
	NewWeapon->SetActorHiddenInGame(true);
	SpawnedWeapons.Add(NewWeapon);

	// 장비창으로 옮겼으니 인벤토리에서는 빠짐
	PlayerCharacter->InventoryComponent->RemoveItem(Item, 1);


	return true;
}

bool UEldenEquipmentComponent::UnequipWeapon(AEldenWeapon* Weapon)
{
	if (!Weapon || !SpawnedWeapons.Contains(Weapon)) return false;

	// 만약 무기가 한종류라면 장착 해제 불가 (맨손 상태 허용 불가)
	if (SpawnedWeapons.Num() <= 1) return false;

	// 장착중인 무기가 해제할 무기인가 (들고 있는 무기를 해제하는 경우엔 다음 무기로 자동 승계)
	bool bWasActiveWeapon = (EquippedWeapon == Weapon) ;

	// 해제하면 이 무기가 인벤토리로 돌아갈 자리가 있는지 먼저 확인
	if (PlayerCharacter->InventoryComponent->GetRemainingCapacity(Weapon->GetItemDefinition()) <= 0) return false;
	
	SpawnedWeapons.Remove(Weapon);
	// 액터를 다시 인벤토리로 반환
	PlayerCharacter->InventoryComponent->AddItem(Weapon->GetItemDefinition(), 1);
	Weapon->Destroy();

	if (bWasActiveWeapon)
	{
		// 해제 직전 검사 Num<=1로 최소 1개는 남아있음이 보장되므로 인덱스 0은 항상 유효
		EquippedWeapon = SpawnedWeapons[0];
		EquippedWeapon->SetActorHiddenInGame(false);
		if (EquippedShield)
		{
			if (EquippedWeapon->GetWeaponStance() == EWeaponStance::TwoHanded)
			{
				EquippedShield->SetActorHiddenInGame(true);
			}
			else if (EquippedWeapon->GetWeaponStance() == EWeaponStance::OneHanded)
			{
				EquippedShield->SetActorHiddenInGame(false);
			}
		}

		// 쥐고 있는 장비가 바뀌었으니 HUD 갱신
		OnEquipmentChanged.Broadcast();
	}

	// 해제된 무기가 배열에서 앞쪽에 있었다면 뒤 원소들 인덱스가 한 칸씩 당겨지므로
	// 활성 무기가 아닌 걸 해제한 경우에도 CurrentWeaponIndex가 어긋날 수 있음 -> 매번 재동기화
	CurrentWeaponIndex = SpawnedWeapons.IndexOfByKey(EquippedWeapon);


	return true;
}

bool UEldenEquipmentComponent::EquipShield(UEldenItemDefinition* Item)
{
	if (!Item || !Item->ShieldClass) return false;
	if (SpawnedShields.Num() >= 5) return false;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = PlayerCharacter;
	SpawnParams.Instigator = PlayerCharacter->GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AEldenShield* NewShield = GetWorld()->SpawnActor<AEldenShield>(Item->ShieldClass,
		PlayerCharacter->GetActorLocation(), PlayerCharacter->GetActorRotation(), SpawnParams);

	if (!NewShield) return false;

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	NewShield->AttachToComponent(PlayerCharacter->GetMesh(), AttachmentRules, FName("LeftHandSocket"));

	NewShield->SetActorHiddenInGame(true);
	SpawnedShields.Add(NewShield);

	PlayerCharacter->InventoryComponent->RemoveItem(Item, 1);

	return true;
}

bool UEldenEquipmentComponent::EquipTalisman(UEldenTalismanDefinition* Item)
{
	if (!Item || EquippedTalismans.Num() >= GetTalismanSlotCount() || EquippedTalismans.Contains(Item)) return false;

	// 조건 통과하면 목록에 추가
	EquippedTalismans.Add(Item);

	// 인벤토리에서 뺀다
	PlayerCharacter->InventoryComponent->RemoveItem(Item, 1);

	// 탈리스만 효과 재계산
	RecalculateTalismanBonus();

	OnEquipmentChanged.Broadcast();
	return true;
}

bool UEldenEquipmentComponent::UnequipTalisman(UEldenTalismanDefinition* Item)
{
	// 아이템이 null 이거나 장착중이 아니거나, 인벤토리로 돌아갈 자리가 없으면 false 리턴
	if (!Item || !EquippedTalismans.Contains(Item) || PlayerCharacter->InventoryComponent->GetRemainingCapacity(Item) <= 0) return false;

	// 조건 통과하면 장착중인 탈리스만 목록에서 해제
	EquippedTalismans.Remove(Item);

	// 인벤토리에 반환
	PlayerCharacter->InventoryComponent->AddItem(Item, 1);

	// 탈리스만 효과 재계산
	RecalculateTalismanBonus();

	OnEquipmentChanged.Broadcast();
	return true;
}

bool UEldenEquipmentComponent::UnequipShield(AEldenShield* Shield)
{
	if (!Shield || !SpawnedShields.Contains(Shield)) return false;

	// 만약 방패가 한종류라면 장착 해제 불가
	if (SpawnedShields.Num() <= 1) return false;

	// 장착중인 방패인가 해제할 방패인가
	bool bWasActiveShield = (EquippedShield == Shield);

	if (PlayerCharacter->InventoryComponent->GetRemainingCapacity(Shield->GetItemDefinition()) <= 0) return false;
	
	SpawnedShields.Remove(Shield);
	PlayerCharacter->InventoryComponent->AddItem(Shield->GetItemDefinition(), 1);
	Shield->Destroy();

	if (bWasActiveShield)
	{
		EquippedShield = SpawnedShields[0];
		EquippedShield->SetActorHiddenInGame(false);
		OnEquipmentChanged.Broadcast();

	}
	// 해제된 방패가 배열에서 앞쪽에 있었다면 뒤 원소들 인덱스가 한 칸씩 당겨지므로
	// 활성 방패가 아닌 걸 해제한 경우에도 CurrentShieldIndex가 어긋날 수 있음 -> 매번 재동기화
	CurrentShieldIndex = SpawnedShields.IndexOfByKey(EquippedShield);

	return true;
}

bool UEldenEquipmentComponent::UnequipWeaponByItem(UEldenItemDefinition* Item)
{
	if (!Item) return false;

	// 장비창 UI는 아이템 데이터만 들고 있으므로, 그에 대응하는 스폰된 액터를 찾아서 위임
	for (const auto& Weapon : SpawnedWeapons)
	{
		if (Weapon->GetItemDefinition() == Item)
		{
			return UnequipWeapon(Weapon);
		}
	}
	return false;
}

bool UEldenEquipmentComponent::UnequipShieldByItem(UEldenItemDefinition* Item)
{
	if (!Item) return false;

	for (const auto& Shield : SpawnedShields)
	{
		if (Shield->GetItemDefinition() == Item)
		{
			return UnequipShield(Shield);
		}
	}
	return false;
}



void UEldenEquipmentComponent::SwitchWeapon()
{
	// 공격 구르기 가드 등 다른 행동 중엔 무기 교체 금지
	if (PlayerCharacter->GetState() != ECharacterState::Idle) return;

	// 무기가 1개 이하면 교체라는 행위 자체가 의미가 없기에 return
	if (SpawnedWeapons.Num() < 2) return;

	// 지금 장착 중인 무기 숨기고
	SpawnedWeapons[CurrentWeaponIndex]->SetActorHiddenInGame(true);

	// 인덱스를 다음 슬롯으로 순환 (배열 끝에서 다시 0으로 돌아오는 원형 순회)
	CurrentWeaponIndex = (CurrentWeaponIndex + 1) % SpawnedWeapons.Num();

	// 바꿀 무기 보이기
	SpawnedWeapons[CurrentWeaponIndex]->SetActorHiddenInGame(false);
	EquippedWeapon = SpawnedWeapons[CurrentWeaponIndex];

	if (EquippedShield)
	{

		if (EquippedWeapon->GetWeaponStance() == EWeaponStance::TwoHanded)
		{
			// 두손 무기로 바꿨다면 방패를 숨긴다.
			EquippedShield->SetActorHiddenInGame(true);
		}
		else if (EquippedWeapon->GetWeaponStance() == EWeaponStance::OneHanded)
		{
			// 두손 무기에서 한손 무기로 되돌아왔을때 방패를 다시 꺼낸다
			// 여기가 빠지면 두손 무기로 바꾸고나서 방패가 영원히 안보이게 되는 버그가 생김
			EquippedShield->SetActorHiddenInGame(false);
		}
	}
	OnEquipmentChanged.Broadcast();

}

void UEldenEquipmentComponent::SwitchShield()
{
	// 공격 구르기 가드 등 다른 행동 중엔 방패 교체 금지
	if (PlayerCharacter->GetState() != ECharacterState::Idle) return;

	if (SpawnedShields.Num() < 2) return;

	// 지금 장착 중인 방패 숨기고
	SpawnedShields[CurrentShieldIndex]->SetActorHiddenInGame(true);

	// 인덱스를 다음 슬롯으로 순환 (배열 끝에서 다시 0으로 돌아오는 원형 순회)
	CurrentShieldIndex = (CurrentShieldIndex + 1) % SpawnedShields.Num();

	EquippedShield = SpawnedShields[CurrentShieldIndex];

	if (EquippedWeapon->GetWeaponStance() == EWeaponStance::TwoHanded)
	{
		EquippedShield->SetActorHiddenInGame(true);
	}
	else if (EquippedWeapon->GetWeaponStance() == EWeaponStance::OneHanded)
	{
		EquippedShield->SetActorHiddenInGame(false);
	}

	OnEquipmentChanged.Broadcast();

}

void UEldenEquipmentComponent::SetEquippedItemsHidden(bool bInHidden)
{
	if (EquippedWeapon)
	{
		if (bInHidden) EquippedWeapon->SetActorHiddenInGame(true);
		else EquippedWeapon->SetActorHiddenInGame(false);
	}

	if (EquippedShield)
	{
		if (bInHidden)
		{
			EquippedShield->SetActorHiddenInGame(true);
		}
		else if (!(EquippedWeapon &&
			EquippedWeapon->GetWeaponStance() == EWeaponStance::TwoHanded))
		{
			// 숨김 해제할 때는 두손 무기 상태가 아닐 때만 다시 방패를 보이게 함.
			EquippedShield->SetActorHiddenInGame(false);
		}
	}
}

// 장착한 탈리스만 목록을 받아서, 실제로 효과가 적용되는 것만 골라서 스탯 종류별로 Flat/Percent 합계를 낸 표를 만들어 냄. 여기서 말하는 표는 Result(반환값)이다.
TMap<EStatModifierTarget, FEldenStatBonus> UEldenEquipmentComponent::BuildTalismanBonus() const
{
	// 최종 결과: 스탯 종류 -> 그 종류의 (Flat 합, Percent 합)
	TMap<EStatModifierTarget, FEldenStatBonus> Result;

	// 계열 이름 -> 그 계열의 최고 등급 탈리스만
	TMap<FName, const UEldenTalismanDefinition*> BestByFamily; 

	// 각 계열에서 누가 제일 최고 등급인지 찾는다.
	for (const UEldenTalismanDefinition* Talisman : EquippedTalismans)
	{
		// NULL 이거나 계열이 없으면 건너뛴다. 계열 없는 탈리스만이 비교할 필요 없음
		if (!Talisman || Talisman->FamilyName == NAME_None) continue;

		// 이 계열의 칸을 맵에서 찾고, 그 칸의 별명을 Best로 받는다
		const UEldenTalismanDefinition*& Best = BestByFamily.FindOrAdd(Talisman->FamilyName);

		// 아직 이 계열의 최고가 없거나, 순회하고 있는 지금 것이 더 높은 등급이면 교체
		// 동률일때는 교체 X
		if (!Best || Talisman->FamilyRank > Best->FamilyRank)
		{
			Best = Talisman;
		}
	}

	// 계열별 최고가 정해졌으니, 효과를 실제로 더하는 단계
	for (const UEldenTalismanDefinition* Talisman : EquippedTalismans)
	{
		if (!Talisman) continue;

		// 계열이 있는데 자기 계열의 최고가 아니면 건너뛴다
		if (Talisman->FamilyName != NAME_None && BestByFamily.FindRef(Talisman->FamilyName) != Talisman) continue;

		// 이 탈리스만의 효과를 하나씩 합산
		for (const FEldenStatModifier& Modifier : Talisman->Modifiers)
		{
			// 파생 스탯 수정 대상 미지정인 칸은 무시한다.
			if (Modifier.Target == EStatModifierTarget::None) continue;

			// 이 스탯 종류의 합계 칸을 찾아서 별명으로 받는다.
			FEldenStatBonus& Bonus = Result.FindOrAdd(Modifier.Target);

			if (Modifier.Op == EModifierOp::Flat)
			{
				// 고정값은 Flat 합계에
				Bonus.Flat += Modifier.Value;
			}
			else if (Modifier.Op == EModifierOp::Percent)
			{
				// 비율은 Percent 합계에
				Bonus.Percent += Modifier.Value;
			}
		}
	}

	// TMap을 값으로 반환해도 복사가 아니라 이동이라 비용 걱정 없음
	return Result;
}

void UEldenEquipmentComponent::RecalculateTalismanBonus()
{
	const TMap<EStatModifierTarget, FEldenStatBonus> Bonus = BuildTalismanBonus();

	UE_LOG(LogTemp, Log, TEXT("[Talisman] 재계산: %d종"), Bonus.Num());
	for (const TPair<EStatModifierTarget, FEldenStatBonus>& Pair : Bonus)
	{
		UE_LOG(LogTemp, Log, TEXT("[Talisman] %s Flat=%.2f Percent=%.2f"), *UEnum::GetValueAsString(Pair.Key), Pair.Value.Flat, Pair.Value.Percent);
	}

	if (!PlayerCharacter || !PlayerCharacter->StatComponent) return;

	PlayerCharacter->StatComponent->SetEquipmentBonus(Bonus);
	
}


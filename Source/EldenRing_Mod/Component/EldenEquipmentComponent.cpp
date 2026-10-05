#include "EldenRing_Mod/Component/EldenEquipmentComponent.h"
#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Item/EldenItemDefinition.h"
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
		PlayerCharacter->RefreshEquipmentUI();
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

bool UEldenEquipmentComponent::UnequipShield(AEldenShield* Shield)
{
	if (!Shield || !SpawnedShields.Contains(Shield)) return false;

	// 만약 방패가 한종류라면 장착 해제 불가
	if (SpawnedShields.Num() <= 1) return false;

	// 장착중인 방패인가 해제할 방패인가
	bool bWasActiveShield = (EquippedShield == Shield);

	SpawnedShields.Remove(Shield);
	PlayerCharacter->InventoryComponent->AddItem(Shield->GetItemDefinition(), 1);
	Shield->Destroy();

	if (bWasActiveShield)
	{
		EquippedShield = SpawnedShields[0];
		EquippedShield->SetActorHiddenInGame(false);
		PlayerCharacter->RefreshEquipmentUI();
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
	PlayerCharacter->RefreshEquipmentUI();
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

	PlayerCharacter->RefreshEquipmentUI();
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

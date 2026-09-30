#include "EldenRing_Mod/Component/EldenEquipmentComponent.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
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


	for (TSubclassOf<AEldenWeapon> SlotClass : WeaponSlots)
	{
		// 이 슬롯만 건너뛰고 나머지 무기는 계속 스폰 (return하면 BeginPlay 전체가 끊김)
		if (!SlotClass) continue;

		// 월드에 무기 액터 생성
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = PlayerCharacter;
		SpawnParams.Instigator = PlayerCharacter->GetInstigator();

		AEldenWeapon* NewWeapon = GetWorld()->SpawnActor<AEldenWeapon>(SlotClass, PlayerCharacter->GetActorLocation(),
			PlayerCharacter->GetActorRotation(), SpawnParams);
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

		if (NewWeapon)
		{
			NewWeapon->AttachToComponent(PlayerCharacter->GetMesh(), AttachmentRules, FName("RightHandSocket"));

			// .Add()의 리턴값 = 방금 이 무기가 배열에서 몇번째로 들어갔는지
			// 이 인덱스가 0이 아니면 (첫 무기가 아니면) 겹쳐 보이지 않게 숨겨둔다.
			int32 NewIndex = SpawnedWeapons.Add(NewWeapon);

			if (NewIndex != 0)
			{
				NewWeapon->SetActorHiddenInGame(true);
			}
		}
	}

	EquippedWeapon = SpawnedWeapons.IsValidIndex(0) ? SpawnedWeapons[0] : nullptr;

	if (ShieldClass != nullptr)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = PlayerCharacter;
		SpawnParams.Instigator = PlayerCharacter->GetInstigator();

		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		EquippedShield = GetWorld()->SpawnActor<AEldenShield>(ShieldClass,
			PlayerCharacter->GetActorLocation(), PlayerCharacter->GetActorRotation(), SpawnParams);

		if (EquippedShield != nullptr)
		{
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

			EquippedShield->AttachToComponent(PlayerCharacter->GetMesh(), AttachmentRules, FName("LeftHandSocket"));
		}
	}
	
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
		else if (!(EquippedWeapon && EquippedWeapon->GetWeaponStance() == EWeaponStance::TwoHanded))
		{
			EquippedShield->SetActorHiddenInGame(false);
		}
	}
}

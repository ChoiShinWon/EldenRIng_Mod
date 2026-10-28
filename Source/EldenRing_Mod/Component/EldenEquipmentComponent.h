#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EldenRing_Mod/EldenStatModifier.h"
#include "EldenEquipmentComponent.generated.h"

class AEldenWeapon;
class AEldenShield;
class AEldenCharacter;
class UEldenTalismanDefinition;
class UEldenItemDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentChangedDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDENRING_MOD_API UEldenEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	// 시작 시 기본 무기/방패를 EquipWeapon/EquipShield로 스폰하고, 각각 인덱스 0을 활성화
	virtual void BeginPlay() override;

	/*=============================================================================
	* Weapon & Shield
	*=============================================================================*/
	// 캐릭터가 시작할 때 기본으로 들고 있을 무기
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UEldenItemDefinition* DefaultWeaponDef;

	// 캐릭터가 시작할 때 기본으로 들고 있을 방패
	UPROPERTY(EditDefaultsOnly, Category = "Shield")
	UEldenItemDefinition* DefaultShieldDef;

	// 실제로 월드에 스폰되어 내 손에 들려있는 무기를 가리키는 포인터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AEldenWeapon* EquippedWeapon;

	// 오른손 로테이션에 장착된 무기 액터 전부
	UPROPERTY()
	TArray<AEldenWeapon*> SpawnedWeapons;

	// 왼손 로테이션에 장착된 방패 액터 전부
	UPROPERTY()
	TArray<AEldenShield*> SpawnedShields;

	// SpawnedWeapons/SpawnedShields 배열에서 지금 활성화된 원소의 인덱스
	int32 CurrentWeaponIndex = 0;
	int32 CurrentShieldIndex = 0;

	// 실제로 월드에 스폰되어 있는 보이거나 숨겨진 방패 중 활성 상태인 것
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shield")
	AEldenShield* EquippedShield;

	/*=============================================================================
	* Talisman
	*=============================================================================*/
	// 탈리스만 슬롯 수
	UPROPERTY(EditDefaultsOnly, Category = "Talisman")
	int32 TalismanSlotCount = 4;

	// 장착 중인 탈리스만 목록 (데이터만 들고 있고 액터는 스폰하지 않음)
	// UPROPERTY 필수: 장착하면 인벤토리 맵에서 RemoveItem으로 빠짐
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Talisman")
	TArray<TObjectPtr<UEldenTalismanDefinition>> EquippedTalismans;

	// [임시] 시작 시 장착해 둘 탈리스만. 장비창 UI 전에 PIE로 효과를 테스트하려는 용도
	UPROPERTY(EditDefaultsOnly, Category = "Talisman")
	TArray<TObjectPtr<UEldenTalismanDefinition>> DefaultTalismans;

public:

	UEldenEquipmentComponent();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEquipmentChangedDelegate OnEquipmentChanged;

	// 인벤토리의 아이템 데이터를 뽑아 무기(방패) 액터를 스폰해 로테이션에 추가
	bool EquipWeapon(UEldenItemDefinition* Item);
	bool EquipShield(UEldenItemDefinition* Item);

	bool EquipTalisman(UEldenTalismanDefinition* Item);
	bool UnequipTalisman(UEldenTalismanDefinition* Item);


	// 스폰된 무기(방패) 액터를 해제해서 인벤토리로 되돌림
	bool UnequipWeapon(AEldenWeapon* Weapon);
	bool UnequipShield(AEldenShield* Shield);

	// UI는 액터가 아니라 아이템 데이터만 들고 있으므로, 그에 대응하는 실제 액터 데이터를 찾아
	// UnequipWeapon/Shield에 위임
	bool UnequipWeaponByItem(UEldenItemDefinition* Item);
	bool UnequipShieldByItem(UEldenItemDefinition* Item);

	// 로테이션 내 다음 무기/방패로 순환 전환 (스위치 키에 바인딩)
	void SwitchWeapon();
	void SwitchShield();


	// 장착 무기 방패 숨기기 (포션 마시는 중 등, 일시적으로 양손을 비워야 할 때 사용)
	void SetEquippedItemsHidden(bool bInHidden);

	// 캐릭터가 장착 중인 무기, 방패를 반환하는 함수
	FORCEINLINE AEldenWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE AEldenShield* GetEquippedShield() const { return EquippedShield; }
	// 장비창 UI가  로테이션 전체 (오른손/왼손 5칸) 그릴 때 사용
	FORCEINLINE const TArray<AEldenWeapon*>& GetSpawnedWeapons() const { return SpawnedWeapons; }
	FORCEINLINE const TArray<AEldenShield*>& GetSpawnedShields() const { return SpawnedShields; }

	// UI가 그릴 때 쓰는 getter 함수
	FORCEINLINE const TArray<TObjectPtr<UEldenTalismanDefinition>>& GetEquippedTalismans() const { return EquippedTalismans; }

	FORCEINLINE int32 GetTalismanSlotCount() const { return TalismanSlotCount; }

private:

	// 플레이어 캐릭터 캐싱
	UPROPERTY()
	AEldenCharacter* PlayerCharacter;

	// 장착 목록에서 계열 중복을 거르고(같은 계열을 FamilyRank가 가장 높은 것 하나만) 효과를 종류별로 합산해 돌려준다.
	// 멤버를 바꾸지 않고 값만 만든는 순수 계산이라 const
	TMap<EStatModifierTarget, FEldenStatBonus> BuildTalismanBonus() const;

	// 장착 변경 시 호출: BuildTalismanBonus 결과를 StatComponent로 넘겨 파생 스탯을 갱신하게 함
	void RecalculateTalismanBonus();

};

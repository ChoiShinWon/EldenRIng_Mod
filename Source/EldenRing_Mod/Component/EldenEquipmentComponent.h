#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EldenEquipmentComponent.generated.h"

class AEldenWeapon;
class AEldenShield;
class AEldenCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDENRING_MOD_API UEldenEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEldenEquipmentComponent();

protected:

	// 시작 시 기본 무기/방패를 EquipWeapon/EquipShield로 스폰하고, 각각 인덱스 0을 활성화
	virtual void BeginPlay() override;

	/*=============================================================================
	* Weapon & Shield
	*=============================================================================*/
	// 캐릭터가 시작할 때 기본으로 들고 있을 무기
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	class UEldenItemDefinition* DefaultWeaponDef;

	// 캐릭터가 시작할 때 기본으로 들고 있을 방패
	UPROPERTY(EditDefaultsOnly, Category = "Shield")
	class UEldenItemDefinition* DefaultShieldDef;

	// 실제로 월드에 스폰되어 내 손에 들려있는 무기를 가리키는 포인터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class AEldenWeapon* EquippedWeapon;

	// 오른손 로테이션에 장착된 무기 액터 전부
	UPROPERTY()
	TArray<class AEldenWeapon*> SpawnedWeapons;

	// 왼손 로테이션에 장착된 방패 액터 전부
	UPROPERTY()
	TArray<class AEldenShield*> SpawnedShields;

	// SpawnedWeapons/SpawnedShields 배열에서 지금 활성화된 원소의 인덱스
	int32 CurrentWeaponIndex = 0;
	int32 CurrentShieldIndex = 0;

	// 실제로 월드에 스폰되어 있는 보이거나 숨겨진 방패 중 활성 상태인 것
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shield")
	class AEldenShield* EquippedShield;

public:
	// 인벤토리의 아이템 데이터를 뽑아 무기(방패) 액터를 스폰해 로테이션에 추가
	bool EquipWeapon(class UEldenItemDefinition* Item);
	bool EquipShield(class UEldenItemDefinition* Item);

	// 스폰된 무기(방패) 액터를 해제해서 인벤토리로 되돌림
	bool UnequipWeapon(class AEldenWeapon* Weapon);
	bool UnequipShield(class AEldenShield* Shield);

	// UI는 액터가 아니라 아이템 데이터만 들고 있으므로, 그에 대응하는 실제 액터 데이터를 찾아
	// UnequipWeapon/Shield에 위임
	bool UnequipWeaponByItem(class UEldenItemDefinition* Item);
	bool UnequipShieldByItem(class UEldenItemDefinition* Item);

	// 로테이션 내 다음 무기/방패로 순환 전환 (스위치 키에 바인딩)
	void SwitchWeapon();
	void SwitchShield();


	// 장착 무기 방패 숨기기 (포션 마시는 중 등, 일시적으로 양손을 비워야 할 때 사용)
	void SetEquippedItemsHidden(bool bInHidden);

	// 캐릭터가 장착 중인 무기, 방패를 반환하는 함수
	FORCEINLINE class AEldenWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE class AEldenShield* GetEquippedShield() const { return EquippedShield; }
	// 장비창 UI가  로테이션 전체 (오른손/왼손 5칸) 그릴 때 사용
	FORCEINLINE const TArray<class AEldenWeapon*>& GetSpawnedWeapons() const { return SpawnedWeapons; }
	FORCEINLINE const TArray<class AEldenShield*>& GetSpawnedShields() const { return SpawnedShields; }
private:
	// 플레이어 캐릭터 캐싱
	UPROPERTY()
	class AEldenCharacter* PlayerCharacter;

	void NotifyEquipmentChanged();
};

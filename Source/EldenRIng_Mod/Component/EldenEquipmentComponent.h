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
	virtual void BeginPlay() override;

	/*=============================================================================
	* Weapon & Shield
	*=============================================================================*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	class UEldenItemDefinition* DefaultWeaponDef;

	UPROPERTY(EditDefaultsOnly, Category = "Shield")
	class UEldenItemDefinition* DefaultShieldDef;

	// 실제로 월드에 스폰되어 내 손에 들려있는 무기를 가리키는 포인터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class AEldenWeapon* EquippedWeapon;

	// 스폰된 무기 액터를 전부를 보관할 배열
	UPROPERTY()
	TArray<class AEldenWeapon*> SpawnedWeapons;

	UPROPERTY()
	TArray<class AEldenShield*> SpawnedShields;

	// 현재 인덱스
	int32 CurrentWeaponIndex = 0;
	int32 CurrentShieldIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shield")
	class AEldenShield* EquippedShield;

public:
	bool EquipWeapon(class UEldenItemDefinition* Item);
	bool UnequipWeapon(class AEldenWeapon* Weapon);

	bool EquipShield(class UEldenItemDefinition* Item);
	bool UnequipShield(class AEldenShield* Shield);

	bool UnequipWeaponByItem(class UEldenItemDefinition* Item);
	bool UnequipShieldByItem(class UEldenItemDefinition* Item);

	void SwitchWeapon();
	void SwitchShield();


	// 장착 무기 방패 숨기기
	void SetEquippedItemsHidden(bool bInHidden);

	// 캐릭터가 장착 중인 무기, 방패를 반환하는 함수
	FORCEINLINE class AEldenWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE class AEldenShield* GetEquippedShield() const { return EquippedShield; }
	FORCEINLINE const TArray<class AEldenWeapon*>& GetSpawnedWeapons() const { return SpawnedWeapons; }
	FORCEINLINE const TArray<class AEldenShield*>& GetSpawnedShields() const { return SpawnedShields; }
private:
	// 플레이어 캐릭터 캐싱
	UPROPERTY()
	class AEldenCharacter* PlayerCharacter;
};

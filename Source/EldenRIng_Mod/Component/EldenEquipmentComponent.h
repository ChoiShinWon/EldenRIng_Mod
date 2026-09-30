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
	// 에디터에서 장착할 무기 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TArray<TSubclassOf<class AEldenWeapon>> WeaponSlots;

	// 실제로 월드에 스폰되어 내 손에 들려있는 무기를 가리키는 포인터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class AEldenWeapon* EquippedWeapon;

	// 스폰된 무기 액터를 전부를 보관할 배열
	UPROPERTY()
	TArray<class AEldenWeapon*> SpawnedWeapons;

	// 현재 인덱스
	int32 CurrentWeaponIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shield")
	TSubclassOf<class AEldenShield> ShieldClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shield")
	class AEldenShield* EquippedShield;

public:	
	void SwitchWeapon();

	// 장착 무기 방패 숨기기
	void SetEquippedItemsHidden(bool bInHidden);

	// 캐릭터가 장착 중인 무기, 방패를 반환하는 함수
	FORCEINLINE class AEldenWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE class AEldenShield* GetEquippedShield() const { return EquippedShield; }

private:
	// 플레이어 캐릭터 캐싱
	UPROPERTY()
	class AEldenCharacter* PlayerCharacter;
};

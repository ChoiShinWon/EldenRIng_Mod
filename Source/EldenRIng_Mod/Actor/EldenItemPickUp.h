#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EldenRing_Mod/Interface/Interactable.h"
#include "EldenItemPickUp.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UEldenItemDefinition;
class AEldenCharacter;

// AActor + Interactable: 상호작용 가능한 오브젝트 공통 패턴
UCLASS()
class ELDENRING_MOD_API AEldenItemPickUp : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	AEldenItemPickUp();

	// 실제 줍기 로직 - 상호작용 키를 눌렀을 때 InteractionComponent가 호출
	virtual void Interact(AEldenCharacter* Player) override;

	// 상호작용 프롬프트 UI에 표시할 텍스트 ("??? 줍기")
	virtual FText GetInteractionPrompt() const override;

protected:
	// 루트. 충돌 판정 전용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* InteractionSphere;

	// 순수 비주얼, 스피어에 자식으로 붙음
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* ItemMesh;

	// 픽업이 줍는 순간 인벤토리로 들어갈 아이템 데이터
	// 레벨 배치 시 인스턴스별로 다르게 지정
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UEldenItemDefinition* ItemDef;

	// 한 번 주울 때 몇 개 들어갈지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PickAmount = 1;

	// 플레이어가 범위에 들어오면 InteractionComponent에 상호작용 가능하도록 등록
	// 실제 줍기는 여기서 안함
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	// 범위를 벗어나면 등록 해제
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};

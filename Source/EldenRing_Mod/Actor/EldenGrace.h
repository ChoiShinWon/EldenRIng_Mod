
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EldenRing_Mod/Interface/Interactable.h"
#include "EldenGrace.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UCameraComponent;

UCLASS()
class ELDENRING_MOD_API AEldenGrace : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	
	AEldenGrace();

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* GraceMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* RespawnPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* InteractionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* GraceCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float CameraBlendTime = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> LevelUpWidgetClass;

	// 위젯을 카메라 블렌드 끝난 뒤 여는 타이머
	FTimerHandle OpenMenuTimerHandle;

	// 타이머 콜백 함수
	void OnMenuDelayed();

	// Interact() 에서 Player 포인터를 잠깐 저장해둬야 콜백에서 접근 가능
	TWeakObjectPtr<class AEldenCharacter> PendingPlayer;
public:
	// 세이브 키. 에디터에서 인스턴스마다 직접 입력 (레벨 안에서 고유해야 함)
	// 비거나 중복되면 이어하기 때 은총을 못 찾아 위치 복원이 조용히 실패
	UPROPERTY(EditInstanceOnly, Category = "Grace")
	FName GraceId;

	const FName& GetGraceId() const { return GraceId; }


	virtual void Interact(class AEldenCharacter* Player) override;
	virtual FText GetInteractionPrompt() const override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FTransform GetRespawnTransform() const;

};

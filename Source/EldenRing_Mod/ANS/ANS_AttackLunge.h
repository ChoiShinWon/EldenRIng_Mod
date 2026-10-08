#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_AttackLunge.generated.h"

UCLASS()
class ELDENRING_MOD_API UANS_AttackLunge : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	// 에디터에서 몽타주별/구간별로 다른 돌진 속도를 지정할 수 있게 노출
	UPROPERTY(EditAnywhere)
	float LungeSpeed = 0.0f;

	// Notify 구간의 시작과 끝 두 시점에 훅을 거는 UAnimNotifyState의 표준 인터페이스
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};

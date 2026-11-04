#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EldenTitleController.generated.h"

class UEldenTitleWidget;

UCLASS()
class ELDENRING_MOD_API AEldenTitleController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// 위젯 클래스 슬롯
	UPROPERTY(EditDefaultsOnly, Category = "Title")
	TSubclassOf<UEldenTitleWidget> WidgetClass;

	UPROPERTY()
	UEldenTitleWidget* CurrentWidget = nullptr;
	
};

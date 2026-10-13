
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EldenDialogueWidget.generated.h"

class UTextBlock;

UCLASS()
class ELDENRING_MOD_API UEldenDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetLine(const FText& NPCName, const FText& Line);

protected:
	// 화자 이름
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Speaker = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Line = nullptr;
};

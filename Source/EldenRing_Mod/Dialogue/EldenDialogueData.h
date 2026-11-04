#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EldenDialogueData.generated.h"

UCLASS()
class ELDENRING_MOD_API UEldenDialogueData : public UDataAsset
{
	GENERATED_BODY()

public:
	FText GetSpeakerName() const;
	int32 GetLineCount() const;
	FText GetLine(int32 Index) const;
	bool IsLastLine(int32 Index) const;

protected:
	// 화자 이름
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
	FText NPCName;

	// 대사 목록
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue", meta = (MultiLine = "true"))
	TArray<FText> Lines;


};

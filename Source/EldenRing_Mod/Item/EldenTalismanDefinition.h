#pragma once

#include "CoreMinimal.h"
#include "EldenRing_Mod/Item/EldenItemDefinition.h"
#include "EldenRing_Mod/EldenStatModifier.h"
#include "EldenTalismanDefinition.generated.h"

UCLASS()
class ELDENRING_MOD_API UEldenTalismanDefinition : public UEldenItemDefinition
{
	GENERATED_BODY()

public:
	UEldenTalismanDefinition();

	// 탈리스만이 주는 효과 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Talisman")
	TArray<FEldenStatModifier> Modifiers;

	// FName이 비교가 정수 비교라 빠르고, TMap 키로 바로 쓸 수 있기에 FName 사용
	// 계열 이름, NAME_None이면 계열 없음, 혼자로 취급한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Talisman")
	FName FamilyName;

	// 같은 계열 안의 등급, 높을수록 강하다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Talisman")
	int32 FamilyRank = 1;
};

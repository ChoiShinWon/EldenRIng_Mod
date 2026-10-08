#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"

/** 게임 내 모든 스탯 계산과 UI 연출을 담당하는 템플릿 */
class FStatUtils
{
public:
	// 스탯 증감 템플릿 (HP, 스태미너 공통)
	template<typename T>
	static void UpdateStat(T& CurrentValue, T MaxValue, T Amount)
	{
		// 여기서 static_cast<T>(0)는 형변환 문법, 컴파일러에게 이 0을 템플릿 타입(T)에 맞춰서 알아서 변신시켜달라고 하는 것임.
		CurrentValue = FMath::Clamp(CurrentValue + Amount, static_cast<T>(0), MaxValue);
	}

	// 고스트 바(잔상) 보간 템플릿
	template<typename T>
	static T InterpGhostValue(T CurrentGhost, T Target, float DeltaTime, float Speed)
	{
		return UKismetMathLibrary::FInterpTo(CurrentGhost, Target, DeltaTime, Speed);
	}
};

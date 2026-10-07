#pragma once

#include "CoreMinimal.h"
#include "Engine/DamageEvents.h"

// 이 공격이 방어 쪽에서 어떻게 처리됐는지, 공격자에게 돌려주는 결과
enum class EDamageResult : uint8
{
	Hit, // 일반 피격
	Blocked, // 가드 성공
	Parried, // 패리 성공
	Dodged // 회피 무적으로 씹힘
};

// FDamageEvent를 상속한 데미지 봉투
struct FEldenDamageEvent : public FDamageEvent
{
	static constexpr int32 ClassID = 100;

	// TakeDamage가 FDamageEvent const& (읽기 전용)으로 받는데 그 안에서 결과를 써야 함
	// const 객체에도 쓸 수 있게 하는 키워드
	mutable EDamageResult Result = EDamageResult::Hit;

	// 내 번호표를 알려준다
	virtual int32 GetTypeID() const override { return ClassID; }
	virtual bool IsOfType(int32 InID) const override
	{
		return (ClassID == InID) || FDamageEvent::IsOfType(InID);
	}

};

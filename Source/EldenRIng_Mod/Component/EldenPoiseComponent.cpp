// 포이즈(균형) 리소스만 관리하는 컴포넌트.
// 깎임 -> 0 도달 시 OnPoiseBroken 브로드캐스트까지가 책임.
// 실제 경직 연출(몽타주 재생, 이동 정지, AI 정지)은 소유 액터가 OnPoiseBroken 을 구독해서 처리한다.
// (StatComponent 의 OnZeroHealth -> Die() 와 동일한 분업)

#include "EldenRing_Mod/Component/EldenPoiseComponent.h"
#include "EldenRing_Mod/StatUtils.h"


UEldenPoiseComponent::UEldenPoiseComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

	// 리젠은 피격후 잠깐만 필요하므로 평소엔 Tick을 꺼둠.
	PrimaryComponentTick.bStartWithTickEnabled = false;
}



void UEldenPoiseComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentPoise = MaxPoise;

}

void UEldenPoiseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 마지막 피격 후 PoiseRegenDelay 동안은 회복 대기 (StartRegenDelay가 타이머로 풀어줌)
	if (!bCanRegen) return;

	CurrentPoise += PoiseRegenRate * DeltaTime;

	CurrentPoise = FMath::Clamp(CurrentPoise, 0.0f, MaxPoise);

	// MaxPoise에 도달하면 Tick 재우기
	if (CurrentPoise >= MaxPoise)
	{
		// 회복이 끝났으면 틱을 재워서 다음 피격 전까지 CPU를 쓰지 않음
		SetComponentTickEnabled(false);
	}
}

void UEldenPoiseComponent::ApplyPoiseDamage(float PoiseDmg)
{

	FStatUtils::UpdateStat(CurrentPoise, MaxPoise, -PoiseDmg);

	StartRegenDelay();

	
	if (CurrentPoise <= 0)
	{
		// 이번 타격으로 균형이 무너짐
		// 즉시 MaxPoise로 되돌려서, 경직이 끝나자마자 남은 데미지로 재브레이크되는 것 방지
		OnPoiseBroken.Broadcast();
		CurrentPoise = MaxPoise;
		return;
	}
	// 포이즈가 깎였으니 리젠 구간 진입 -> 틱을 깨움
	SetComponentTickEnabled(true);
}

void UEldenPoiseComponent::StartRegenDelay()
{
	bCanRegen = false;
	// WeakLambda: 타이머 발화 전에 컴포넌트가 파괴되면 this가 무효
	// 그 경우 람다를 실행않음
	// 월드 타이머 매니저는 액터 소멸 시 자동 정리되지 않으므로 방어
	GetWorld()->GetTimerManager().SetTimer(
		RegenDelayTimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this] {bCanRegen = true; }),
		PoiseRegenDelay, false);

}




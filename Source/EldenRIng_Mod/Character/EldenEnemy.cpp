

#include "EldenRing_Mod/Character/EldenEnemy.h"
#include "EldenRing_Mod/StatUtils.h"
#include "Components/WidgetComponent.h"


AEldenEnemy::AEldenEnemy()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	TargetMarkWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TargetMarkWidget"));
	TargetMarkWidget->SetupAttachment(GetMesh());

	TargetMarkWidget->SetWidgetSpace(EWidgetSpace::Screen);
	TargetMarkWidget->SetDrawAtDesiredSize(true);
	TargetMarkWidget->SetVisibility(false);
}


void AEldenEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// 게임이 시작되면 현재 체력을 최대 체력으로 꽉 채워줌.
	CurrentHealth = MaxHealth;
	
}

float AEldenEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator,
	class AActor* DamageCauser)
{
	if (bIsDead) return 0.0f;

	// 부모 클래스의 기본 데미지 로직 실행
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// 스탯 처리 템플릿 적용 (FStatUtils)
	FStatUtils::UpdateStat(CurrentHealth, MaxHealth, -ActualDamage);

	// 로그 띄우기 (맞았는지 확인)
	UE_LOG(LogTemp, Warning, TEXT("몬스터가 %f의 데미지를 입음. 남은 체력: %f"), ActualDamage, CurrentHealth);
	
	if (CurrentHealth <= 0.0f)
	{
		bIsDead = true;
		UE_LOG(LogTemp, Warning, TEXT("몬스터가 죽었다!"));
		// 추후에 사망 애니메이션 추가

		Destroy(); // 몬스터 제거 (임시)
	}
	else if (HitReactMontage)
	{
		// 맞는 모션 재생
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(HitReactMontage);
		}
	}
	return ActualDamage;
}

void AEldenEnemy::ShowTargetMark(bool bShow)
{
	if (TargetMarkWidget)
	{
		TargetMarkWidget->SetVisibility(bShow);
	}
}

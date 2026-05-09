

#include "EldenRing_Mod/Character/EldenEnemy.h"


AEldenEnemy::AEldenEnemy()
{
 	
	PrimaryActorTick.bCanEverTick = true;

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
	// 부모 클래스의 기본 데미지 로직 실행
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// 체력 깎기
	CurrentHealth -= ActualDamage;
	
	// 로그 띄우기 (맞았는지 확인)
	UE_LOG(LogTemp, Warning, TEXT("몬스터가 %f의 데미지를 입음. 남은 체력: %f"), ActualDamage, CurrentHealth);
	
	// 체력이 남아있고, 피격 몽타주가 설정되어 있다면 재생
	if (CurrentHealth > 0.0f && HitReactMontage)
	{
		// 몬스터의 애니메이션 인스턴스를 가져와서 몽타주를 재생
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(HitReactMontage);
		}
	}
	// 만약 체력이 0이 되면? 죽음 처리
	else if (CurrentHealth <= 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("몬스터 사망."));
		// 임시로 액터 파괴
		Destroy();
	}
	return ActualDamage;
}

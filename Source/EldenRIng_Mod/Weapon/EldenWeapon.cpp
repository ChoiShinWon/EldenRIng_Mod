// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenRing_Mod/Weapon/EldenWeapon.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Sound/SoundBase.h"

// Sets default values
AEldenWeapon::AEldenWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// 외형 컴포넌트 생성
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	// 메쉬를 무기의 가장 최상위 부모로 설정
	RootComponent = WeaponMesh;
	// 일단 장착 전에는 칼날이 캐릭터를 밀어내지 않도록 충돌을 끔
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// 박스 컴포넌트 생성 및 무기에 부착
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(GetRootComponent());
	
	// 콜리전 세팅
	// 가만히 들고 뛰어다닐 때 때리면 안되므로, 일단 충돌 꺼둠
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	
	// 캐릭터 자신을 때리면 안되므로 Pawn과의 충돌은 초기에 무시
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

}



void AEldenWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	// 무언가에 닿았을 때 OnBoxOverlap 함수를 실행해 달라고 엔진에 구독(Bind)
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AEldenWeapon::OnBoxOverlap);
}

void AEldenWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 내가 아닌 다른 액터를 때리면
	if (OtherActor && OtherActor != GetOwner())
	{
		// 다중 히트 방지
		if (AlreadyHitActors.Contains(OtherActor)) return;
		// 명단에 없으면 새로 추가
		AlreadyHitActors.Add(OtherActor);
		
		// 타격 지점 찾기
		FVector HitLocation;
		
		if (bFromSweep)
		{
			HitLocation = SweepResult.ImpactPoint;
		}
		else
		{
			{
				// 적의 몸통 표면 중에서, 내 무기 박스 중심점과 가장 가까운 지점을 찾음
				OtherComp->GetClosestPointOnCollision(WeaponBox->GetComponentLocation(), HitLocation);
			}
		}
		// --- [시각적 확인: 빨간 구슬 그리기] ---
		// (월드, 위치, 반지름 10짜리 구슬, 각형 12, 빨간색, 영구지속 안 함, 2초 동안 띄움)
		DrawDebugSphere(GetWorld(), HitLocation, 10.0f, 12, FColor::Red, false, 2.0f);
		
		if (HitParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, HitLocation);
		}
		
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, HitLocation);
		}

		// 무기 고유 기본 데미지
		float FinalDamage = BaseDamage;

		// 여기에 캐릭터의 스탯에 접근해서 AttackPower를 가져와 최종 데미지를 계산
		if (AEldenCharacter* MyOwner = Cast<AEldenCharacter>(GetOwner()))
		{
			if (UEldenStatComponent* OwnerStats = MyOwner->StatComponent)
			{
				FinalDamage += OwnerStats->AttackPower;
			}
		}

		
		// 데미지 전달
		UGameplayStatics::ApplyDamage(OtherActor, FinalDamage, GetInstigatorController(), this, UDamageType::StaticClass());
		// 일단 데미지 로그 띄워놓기 나중엔 여기에 데미지를 줌
		UE_LOG(LogTemp, Warning, TEXT("때렸다! 맞은 액터: %s / 최종 데미지: %f"), *OtherActor->GetName(), FinalDamage);
	}
}

void AEldenWeapon::EnableWeaponCollision()
{
	ClearHitActors();

	// 콜리전 설정
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}


void AEldenWeapon::DisableWeaponCollision()
{
	// 노콜리전으로 설정
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
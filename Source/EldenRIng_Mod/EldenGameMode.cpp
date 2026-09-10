
#include "EldenRing_Mod/EldenGameMode.h"
#include "EldenRing_Mod/Actor/EldenGrace.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Character/EldenEnemy.h"
#include "EldenRing_Mod/Actor/Bloodstain.h"
#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

AEldenGameMode::AEldenGameMode()
{

}

void AEldenGameMode::BeginPlay()
{
	Super::BeginPlay();

	CachedPC = UGameplayStatics::GetPlayerController(this, 0);

	CachedPlayer = Cast<AEldenCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (CachedPlayer)
	{
		CachedPlayer->OnPlayerDied.AddDynamic(this, &AEldenGameMode::HandlePlayerDeath);
		InitialSpawnTransform = CachedPlayer->GetActorTransform();
	}

	for (TActorIterator<AEldenEnemy> It(GetWorld()); It; ++It)
	{
		FEnemySpawnInfo EnemyInfo;
		EnemyInfo.EnemyClass = It->GetClass();
		EnemyInfo.SpawnTransform = It->GetActorTransform();
		EnemySpawnSnapshot.Add(EnemyInfo);
	}

}

void AEldenGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(RespawnTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void AEldenGameMode::RegisterGrace(AEldenGrace* Grace)
{
	if (!Grace) return;
	LastGrace = Grace;
	UE_LOG(LogTemp, Log, TEXT("마지막 은총 등록: %s"), *GetNameSafe(Grace));
}



void AEldenGameMode::HandlePlayerDeath(AEldenCharacter* DeadPlayer)
{
	DropBloodstain(DeadPlayer);

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("플레이어 사망 감지 - 리스폰 대기"));
	UE_LOG(LogTemp, Warning, TEXT("HandlePlayerDeath: %s"), *GetNameSafe(DeadPlayer));
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &AEldenGameMode::RespawnPlayer,
		RespawnDelay, false);

	if (CachedPC && CachedPC->PlayerCameraManager)
	{
		// 투명 -> 불투명(검정), 암전 시간
		CachedPC->PlayerCameraManager->StartCameraFade(0.f, 1.f, 1.5f, FLinearColor::Black, false, true);
	}

	if (YouDiedWidgetClass && !SpawnedWidget)
	{
		SpawnedWidget = CreateWidget<UUserWidget>(GetWorld(), YouDiedWidgetClass);
		if (SpawnedWidget)
		{
			SpawnedWidget->AddToViewport(10);
		}
	}
}

void AEldenGameMode::RespawnPlayer()
{
	if (!CachedPlayer) return;

	FTransform Target = LastGrace.IsValid() ? LastGrace->GetRespawnTransform() : InitialSpawnTransform;
	CachedPlayer->Revive(Target);

	// 위젯 제거 + null
	if (SpawnedWidget)
	{
		SpawnedWidget->RemoveFromParent();
		SpawnedWidget = nullptr;
	}

	// 페이드인 작업
	if (CachedPC && CachedPC->PlayerCameraManager)
	{
		CachedPC->PlayerCameraManager->StartCameraFade(1.f, 0.f, 1.f, FLinearColor::Black, false, false);
	}
}

void AEldenGameMode::DropBloodstain(AEldenCharacter* DeadPlayer)
{

	// 널가드
	if (!DeadPlayer || !DeadPlayer->StatComponent) return;


	// 라인 트레이스 광선의 시작점과 끝점
	const FVector StartLoc = DeadPlayer->GetActorLocation();
	const FVector EndLoc = StartLoc - FVector(0.f, 0.f, 500.f);

	// 결과를 담아올 빈 상자
	FHitResult Hit;

	// 광선이 플레이어 자신의 캡슐 메시에 먼저 맞으면 ImpactPoint가 시체 몸통이 되어버리기때문에
	// 이 액터는 무시 등록
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(DeadPlayer);

	// 트레이스 실행
	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		StartLoc, EndLoc,
		ECC_Visibility,
		Params);

	// 스폰 위치
	FVector SpawnLoc = bHit ? Hit.ImpactPoint : StartLoc;

	// 회수 안 된 이전 블러드스테인 영구 소멸
	if (ActiveBloodstain.IsValid()) ActiveBloodstain->Destroy();
	// 손실 룬 확정
	int32 Lost = DeadPlayer->StatComponent->CurrentRunes;
	// 남은 룬이 없다면 실행 X
	if (Lost <= 0) return;

	// 스폰
	FActorSpawnParameters Params2;
	Params2.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABloodstain* Spawned = GetWorld()->SpawnActor<ABloodstain>(
		BloodstainClass, SpawnLoc, FRotator::ZeroRotator, Params2);

	if (!Spawned) return;
	// 손실 룬을 Bloodstain에 전달
	Spawned->InitBloodstain(Lost);
	ActiveBloodstain = Spawned;

	// 플레이어 룬 0으로
	DeadPlayer->StatComponent->LoseAllRunes();
}

void AEldenGameMode::ResetAllEnemies()
{
	// 파괴 패스
	// TActorIterator 순회 도중 Destroy() 하면 이터레이터가 불안정해질 수 있어
	// 먼저 전부 배열에 모아두고, 순회가 끝난 뒤 따로 파괴
	TArray<AEldenEnemy*> TempArray;
	for (TActorIterator<AEldenEnemy> It(GetWorld()); It; ++It)
	{
		TempArray.Add(*It);
	}
	for (AEldenEnemy* E : TempArray)
	{
		if (IsValid(E)) E->Destroy(); // 살아있든 래그돌이든 무조건 제거
	}

	// 스폰 패스
	for (const FEnemySpawnInfo& Info : EnemySpawnSnapshot)
	{
		FActorSpawnParameters SpawnPar;
		SpawnPar.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector Loc = Info.SpawnTransform.GetLocation();
		FQuat Q = Info.SpawnTransform.GetRotation();
		FRotator Rot = Q.Rotator();

		GetWorld()->SpawnActor<AEldenEnemy>(Info.EnemyClass, Loc, Rot, SpawnPar);
	}
}

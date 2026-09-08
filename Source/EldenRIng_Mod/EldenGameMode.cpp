
#include "EldenRing_Mod/EldenGameMode.h"
#include "EldenRing_Mod/Actor/EldenGrace.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

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

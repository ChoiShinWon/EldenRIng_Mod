
#include "EldenRing_Mod/EldenGameMode.h"
#include "EldenRing_Mod/Actor/EldenGrace.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "Kismet/GameplayStatics.h"

AEldenGameMode::AEldenGameMode()
{

}

void AEldenGameMode::BeginPlay()
{
	Super::BeginPlay();

	AEldenCharacter* PC = Cast<AEldenCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PC)
	{
		PC->OnPlayerDied.AddDynamic(this, &AEldenGameMode::HandlePlayerDeath);
		InitialSpawnTransform = PC->GetActorTransform();
	}

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
}

void AEldenGameMode::RespawnPlayer()
{
	AEldenCharacter* PC = Cast<AEldenCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!PC) return;

	FTransform Target = LastGrace.IsValid() ? LastGrace->GetRespawnTransform() : InitialSpawnTransform;
	PC->Revive(Target);
}

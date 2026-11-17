
#include "EldenRing_Mod/EldenGameMode.h"
#include "EldenRing_Mod/Actor/EldenGrace.h"
#include "EldenRing_Mod/Actor/EldenItemPickUp.h"
#include "EldenRing_Mod/Character/EldenCharacter.h"
#include "EldenRing_Mod/Character/EldenEnemy.h"
#include "EldenRing_Mod/Save/EldenSaveSubsystem.h"
#include "EldenRing_Mod/Save/EldenSaveGame.h"
#include "EldenRing_Mod/Actor/Bloodstain.h"
#include "EldenRing_Mod/Actor/EldenNPC.h"
#include "EldenRing_Mod/Component/EldenStatComponent.h"
#include "EldenRing_Mod/Component/EldenInventoryComponent.h"
#include "EldenRing_Mod/Component/EldenEquipmentComponent.h"
#include "Components/AudioComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

AEldenGameMode::AEldenGameMode()
{

}

void AEldenGameMode::BeginPlay()
{
	Super::BeginPlay();
	StartBGM();

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
		EnemyInfo.Instance = *It;
		EnemySpawnSnapshot.Add(EnemyInfo);
		BindEnemy(*It);
	}

	GetWorldTimerManager().SetTimerForNextTick(this, &AEldenGameMode::ApplySavedProgress);
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
	if (GetWorldTimerManager().IsTimerActive(RespawnTimerHandle)) return;
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

void AEldenGameMode::HandleEnemyDied(int32 RuneReward, APawn* Killer)
{
	AEldenCharacter* Player = Cast<AEldenCharacter>(Killer);
	if (!Player) return;

	if (Player->StatComponent)
	{
		Player->StatComponent->AddRunes(RuneReward);
	}

	OnEnemyKilled.Broadcast();
}

void AEldenGameMode::BindEnemy(AEldenEnemy* Enemy)
{
	if (!Enemy) return;
	Enemy->OnEnemyDied.AddDynamic(this, &AEldenGameMode::HandleEnemyDied);
}

void AEldenGameMode::RespawnPlayer()
{
	if (!CachedPlayer) return;

	ResetAllEnemies();

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
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(DeadPlayer);

	// 트레이스 실행
	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		StartLoc, EndLoc,
		ECC_Visibility,
		TraceParams);


	// 회수 안 된 이전 블러드스테인 영구 소멸
	if (ActiveBloodstain.IsValid()) ActiveBloodstain->Destroy();
	// 손실 룬 확정
	int32 Lost = DeadPlayer->StatComponent->CurrentRunes;
	// 남은 룬이 없다면 실행 X
	if (Lost <= 0) return;

	const FVector SpawnLoc = bHit ? Hit.ImpactPoint : StartLoc;

	if (!SpawnBloodstain(SpawnLoc, Lost)) return;
	// 플레이어 룬 0으로
	DeadPlayer->StatComponent->LoseAllRunes();
}

bool AEldenGameMode::SpawnBloodstain(const FVector& Location, int32 Runes)
{
	if (!BloodstainClass) return false;


	// 스폰
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABloodstain* Spawned = GetWorld()->SpawnActor<ABloodstain>(
		BloodstainClass, Location, FRotator::ZeroRotator, SpawnParams);

	if (!Spawned) return false;
	// 손실 룬을 Bloodstain에 전달
	Spawned->InitBloodstain(Runes);
	ActiveBloodstain = Spawned;

	return true;

}

void AEldenGameMode::ResetAllEnemies()
{
	for (FEnemySpawnInfo& Info : EnemySpawnSnapshot)
	{
		if (Info.Instance.IsValid() && !Info.Instance->GetIsDead())
		{
			Info.Instance->ResetToSpawn(Info.SpawnTransform);
			continue;
		}

		if (Info.Instance.IsValid())
		{
			Info.Instance->Destroy();
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector Loc = Info.SpawnTransform.GetLocation();
		FQuat Q = Info.SpawnTransform.GetRotation();
		FRotator Rot = Q.Rotator();

		AEldenEnemy* SpawnedEnemy = GetWorld()->SpawnActor<AEldenEnemy>(Info.EnemyClass, Loc, Rot, SpawnParams);
		BindEnemy(SpawnedEnemy);
		Info.Instance = SpawnedEnemy;

	}

	
}

void AEldenGameMode::HandleGraceRest(AEldenGrace* Grace, AEldenCharacter* Player)
{
	RegisterGrace(Grace);
	ResetAllEnemies();
	OnGraceRested.Broadcast();

	if (!Player || !Player->InventoryComponent) return;

	Player->InventoryComponent->RefillPotions();


	SaveProgress();

}

void AEldenGameMode::SaveProgress()
{
	if (!CachedPlayer || !LastGrace.IsValid()) return;
	UEldenSaveSubsystem* Sub = GetGameInstance()->GetSubsystem<UEldenSaveSubsystem>();
	if (!Sub) return;

	UEldenSaveGame* Save = Sub->GetOrCreateSave();
	if (!Save) return;

	CachedPlayer->StatComponent->CaptureTo(Save->Stats);

	CachedPlayer->InventoryComponent->CaptureTo(Save->SavedInventory);
	CachedPlayer->EquipmentComponent->CaptureTo(Save->SavedEquipment);

	Save->LastGraceId = LastGrace->GetGraceId();
	Save->SavedWorld.PickupIDs = CollectedPickups;

	// 맵에 칸을 만들고 채우기
	// 없으면 칸을 새로 만들어 주므로 항상 존재 -> 참조로 받아 맵 안의 칸을 직접 채움
	for (TActorIterator<AEldenNPC> It(GetWorld()); It; ++It)
	{
		FEldenSavedNPC& Entry = Save->SavedWorld.NPCs.FindOrAdd(It->GetFName());
		It->CaptureTo(Entry);
	}

	// 회수 전의 핏자국이 있으면 위치와 룬을 저장
	if (ActiveBloodstain.IsValid())
	{
		ActiveBloodstain->CaptureTo(Save->SavedWorld.Bloodstain);
	}
	// 없으면 없음(Runes = 0)을 덮어씀. 안 하면 재사용되는 Save에 옛 핏자국이 남아 이어하기 때 되살아남
	else
	{
		FEldenSavedBloodstain Temp;
		Save->SavedWorld.Bloodstain = Temp;
	}

	const bool bOk = Sub->WriteToDisk();
	UE_LOG(LogTemp, Log, TEXT("[Save] 은총 %s 저장 %s"),
		*Save->LastGraceId.ToString(), bOk ? TEXT("성공") : TEXT("실패"));
}

void AEldenGameMode::ApplySavedProgress()
{
	if (!CachedPlayer) return;

	UEldenSaveSubsystem* Sub = GetGameInstance()->GetSubsystem<UEldenSaveSubsystem>();
	if (!Sub) return;

	UEldenSaveGame* Save = Sub->GetCurrentSave();
	if (!Save) return;

	CachedPlayer->InventoryComponent->ApplyFrom(Save->SavedInventory);
	CachedPlayer->EquipmentComponent->ApplyFrom(Save->SavedEquipment);
	CachedPlayer->StatComponent->ApplyFrom(Save->Stats);

	CollectedPickups = Save->SavedWorld.PickupIDs;
	// 이어하기 시 레벨이 새로 열려서 배치된 픽업이 전부 다시 생겨남
	// 저장된 "이미 주운 ID" 집합에 있는 픽업은 월드에서 제거
	for (TActorIterator<AEldenItemPickUp> It(GetWorld()); It; ++It)
	{
		if (CollectedPickups.Contains(It->GetFName()))
		{
			It->Destroy();
		}
	}

	// 칸이 없을 수 있음(옛 세이브 등) -> 포인터로 받고 nullptr이면 건너뜀, 읽기만 하니 const
	for (TActorIterator<AEldenNPC> It(GetWorld()); It; ++It)
	{
		const FEldenSavedNPC* SavedNPC = Save->SavedWorld.NPCs.Find(It->GetFName());
		if (!SavedNPC) continue;
		It->ApplyFrom(*SavedNPC);
	}

	const FEldenSavedBloodstain& SavedBloodstain = Save->SavedWorld.Bloodstain;

	if (SavedBloodstain.Runes > 0)
	{
		SpawnBloodstain(SavedBloodstain.SpawnLocation, SavedBloodstain.Runes);
	}

	AEldenGrace* Found = nullptr;
	for (TActorIterator<AEldenGrace> It(GetWorld()); It; ++It)
	{
		if (It->GetGraceId() == Save->LastGraceId)
		{
			Found = *It;
			break;
		}
	}
	if (!Found)
	{
		UE_LOG(LogTemp, Warning, TEXT("마지막 은총을 찾지 못함: ID %s"), *Save->LastGraceId.ToString());
		return;
	}


	RegisterGrace(Found);

	const FTransform T = Found->GetRespawnTransform();
	CachedPlayer->SetActorLocationAndRotation(T.GetLocation(), T.GetRotation().Rotator(), false, nullptr, ETeleportType::TeleportPhysics);

	
}

void AEldenGameMode::MarkPickupCollected(FName ID)
{
	if (ID == NAME_None) return;
	CollectedPickups.Add(ID);
}

void AEldenGameMode::StartBGM()
{
	if (!MainBGM) return;
	BGMComponent = UGameplayStatics::CreateSound2D(this, MainBGM);

	if (BGMComponent) BGMComponent->FadeIn(FadeInTime);
}

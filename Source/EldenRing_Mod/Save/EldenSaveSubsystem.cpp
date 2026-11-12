
#include "EldenRing_Mod/Save/EldenSaveSubsystem.h"
#include "EldenRing_Mod/Save/EldenSaveGame.h"
#include "Kismet/GameplayStatics.h"

bool UEldenSaveSubsystem::HasSave() const
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex);
	
}

bool UEldenSaveSubsystem::LoadFromDisk()
{
	USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex);
	CurrentSave = Cast<UEldenSaveGame>(SaveGame);
	return CurrentSave != nullptr;
}

bool UEldenSaveSubsystem::WriteToDisk()
{
	if (CurrentSave == nullptr) return false;
	const bool bOk = UGameplayStatics::SaveGameToSlot(CurrentSave, SlotName, UserIndex);
	
	UE_LOG(LogTemp, Log, TEXT("[Save] 슬롯 %s 쓰기 %s"),
		SlotName, bOk ? TEXT("성공") : TEXT("실패"));
	return bOk;
}

UEldenSaveGame* UEldenSaveSubsystem::GetOrCreateSave()
{
	if (CurrentSave) return CurrentSave;

	USaveGame* SaveGame = UGameplayStatics::CreateSaveGameObject(UEldenSaveGame::StaticClass());
	CurrentSave = Cast<UEldenSaveGame>(SaveGame);
	return CurrentSave;
}

void UEldenSaveSubsystem::ClearCurrentSave()
{
	CurrentSave = nullptr;
}

UEldenSaveGame* UEldenSaveSubsystem::GetCurrentSave() const
{
	return CurrentSave;
}

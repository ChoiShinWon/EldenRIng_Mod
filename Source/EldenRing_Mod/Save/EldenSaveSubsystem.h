#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EldenSaveSubsystem.generated.h"

class UEldenSaveGame;

UCLASS()
class ELDENRING_MOD_API UEldenSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// 디스크에 세이브 파일이 있는가
	bool HasSave() const;

	// 디스크에서 읽어 CurrentSave에 보관. 성공 여부 반환.
	bool LoadFromDisk();

	// 메모리의 CurrentSave를 디스크에 씀. 성공 여부 반환
	bool WriteToDisk();

	// 메모리 세이브 객체를 돌려줌. 없으면 새로 만들어서 CurrentSave에 보관하고 돌려줌
	UEldenSaveGame* GetOrCreateSave();

	// 새 게임 시작 시 메모리의 세이브를 비움
	void ClearCurrentSave();

	UEldenSaveGame* GetCurrentSave() const;

private:
	// 읽어온/만들어 둔 세이브 객체
	UPROPERTY()
	TObjectPtr<UEldenSaveGame> CurrentSave;

	// 슬롯 이름과 유저 인덱스 (매직 스트링을 여기 한 곳에만)
	static constexpr const TCHAR* SlotName = TEXT("EldenSave");

	static constexpr int32 UserIndex = 0;
};

// Copyright Epic Games, Inc. All Rights Reserved.

#include "EldenRIng_ModGameMode.h"
#include "EldenRIng_ModCharacter.h"
#include "UObject/ConstructorHelpers.h"

AEldenRIng_ModGameMode::AEldenRIng_ModGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

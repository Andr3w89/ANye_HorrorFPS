// Copyright Epic Games, Inc. All Rights Reserved.

#include "ANye_HorrorFPSGameMode.h"
#include "ANye_HorrorFPSCharacter.h"
#include "UObject/ConstructorHelpers.h"

AANye_HorrorFPSGameMode::AANye_HorrorFPSGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}

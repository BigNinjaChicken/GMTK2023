// Copyright Epic Games, Inc. All Rights Reserved.

#include "GMTK2023GameMode.h"
#include "GMTK2023Character.h"
#include "UObject/ConstructorHelpers.h"

AGMTK2023GameMode::AGMTK2023GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

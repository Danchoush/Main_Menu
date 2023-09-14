// Copyright Epic Games, Inc. All Rights Reserved.

#include "Lab4GameMode.h"
#include "Lab4Character.h"
#include "UObject/ConstructorHelpers.h"

ALab4GameMode::ALab4GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

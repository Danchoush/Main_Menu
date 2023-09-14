#pragma once

#include "CoreMinimal.h"
#include "../Lab4GameMode.h"
#include "Lab4/GameInstances/Lab4GameInstance.h"
#include "LobbyGameMode.generated.h"

UCLASS()
class LAB4_API ALobbyGameMode : public ALab4GameMode
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void StartGame();

private:
	const FString TravelGamePath = TEXT("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");
};

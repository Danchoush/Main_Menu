#include "LobbyGameMode.h"

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	Cast<ULab4GameInstance>(GetGameInstance())->ConnectLobbyGameMode(this);
}

void ALobbyGameMode::StartGame()
{
	bUseSeamlessTravel = true;
	GetWorld()->ServerTravel(TravelGamePath);
}

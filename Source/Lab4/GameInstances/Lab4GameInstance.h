#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Lab4GameInstance.generated.h"

UCLASS()
class LAB4_API ULab4GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	
	void ConnectMainMenuInitializer(class AMainMenuInitializer* const pInitializer);
	void ConnectLobbyGameMode(class ALobbyGameMode* const pLobby);

	FString GetPlayerName();

	void SetPlayerName(const FString& Name);
	void SetServerName(const FString& SessionName);
	void SetJoinIndex(const uint32 Index);

	void AddCharacter(class ALab4Character* const pCharacter, const FString& Name);
	void RemoveCharacter(class ALab4Character* const pCharacter);

	void KillPlayer(class ALab4Character* const pCharacter);
	
	void Host() const;
	void Join() const;

	void StartSession();
	void StartMatch();

	void RefreshServersList();
	
	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool Success);
	void OnJoinSessionComplete(FName Name, EOnJoinSessionCompleteResult::Type Result);
	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

	bool GetIsInGame() const;
	
private:
	void CreateSession() const;
	void LoadMainMenu() const;

	int32 CalculateAlivePlayers() const;
	
	UPROPERTY()
	class AMainMenuInitializer *m_pMainMenu;

	UPROPERTY()
	class ALobbyGameMode *m_pLobby;
	
	IOnlineSessionPtr m_pSessionInterface;
	TSharedPtr<FOnlineSessionSearch> m_pSessionSearch;

	FString m_PlayerName;
	FName m_ServerName;
	uint32 m_JoinIndex;

	bool m_bIsInGame;

	TArray<FString> m_PlayerNames;
	TArray<class ALab4Character*> m_Characters;
	TArray<bool> m_AlivePlayers;
	
	TSubclassOf<UUserWidget> m_ConnectionFailureWidget;

	const FName ServerNameKey = "ServerName";
	const FName SessionNameConst = "Session";
	const FString TravelLobbyPath = TEXT("/Game/Lobby/Lobby?listen");
	const FString TravelMainMenuPath = TEXT("/Game/MainMenu/MainMenuMap");
	const FString TravelGamePath = TEXT("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");
};

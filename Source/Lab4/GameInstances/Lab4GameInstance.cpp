#include "Lab4GameInstance.h"

#include "Blueprint/UserWidget.h"
#include "Lab4/Lab4Character.h"
#include "Lab4/Actors/MainMenuInitializer.h"
#include "Lab4/GameModes/LobbyGameMode.h"

void ULab4GameInstance::Init()
{
	const IOnlineSubsystem *pSubsystem = IOnlineSubsystem::Get();

	if (pSubsystem == nullptr) return;
	
	m_pSessionInterface = pSubsystem->GetSessionInterface();

	if(!m_pSessionInterface.IsValid()) return;
	
	m_pSessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &ULab4GameInstance::OnCreateSessionComplete);
	m_pSessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &ULab4GameInstance::OnDestroySessionComplete);
	m_pSessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &ULab4GameInstance::OnJoinSessionComplete);
	m_pSessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &ULab4GameInstance::OnFindSessionsComplete);

	if (GEngine != nullptr)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &ULab4GameInstance::OnNetworkFailure);
	}

	m_bIsInGame = false;
}

void ULab4GameInstance::ConnectMainMenuInitializer(AMainMenuInitializer* const pInitializer)
{
	m_pMainMenu = pInitializer;
}

void ULab4GameInstance::ConnectLobbyGameMode(ALobbyGameMode* const pLobby)
{
	m_pLobby = pLobby;
}

FString ULab4GameInstance::GetPlayerName()
{
	return m_PlayerName;
}

void ULab4GameInstance::SetPlayerName(const FString& Name)
{
	m_PlayerName = Name;
}

void ULab4GameInstance::SetServerName(const FString& SessionName)
{
	m_ServerName = FName(SessionName);
}

void ULab4GameInstance::SetJoinIndex(const uint32 Index)
{
	m_JoinIndex = Index;
}

void ULab4GameInstance::AddCharacter(ALab4Character* const pCharacter, const FString& Name)
{
	m_PlayerNames.Add(Name);
	m_Characters.Add(pCharacter);
	m_AlivePlayers.Add(true);

	for (int i = 0; i < m_Characters.Num(); ++i)
	{
		m_Characters[i]->SetPlayerNames(m_PlayerNames);
	}
}

void ULab4GameInstance::RemoveCharacter(ALab4Character* const pCharacter)
{
	const int32 Index = m_Characters.Find(pCharacter);

	if (Index == INDEX_NONE) return;

	m_Characters.RemoveAt(Index);
	m_PlayerNames.RemoveAt(Index);
	m_AlivePlayers.RemoveAt(Index);
	
	for (int i = 0; i < m_Characters.Num(); ++i)
	{
		m_Characters[i]->SetPlayerNames(m_PlayerNames);
	}
	
	if (CalculateAlivePlayers() <= 1)
	{
		for (int8 i = 0; i < m_Characters.Num(); ++i)
		{
			m_AlivePlayers[i] = true;
		}
		
		GetWorld()->ServerTravel(TravelGamePath);
	}
}

void ULab4GameInstance::KillPlayer(ALab4Character* const pCharacter)
{
	const int32 Index = m_Characters.Find(pCharacter);

	if (m_AlivePlayers[Index])
	{
		m_AlivePlayers[Index] = false;
	}

	if (CalculateAlivePlayers() <= 1)
	{
		for (int8 i = 0; i < m_Characters.Num(); ++i)
		{
			m_AlivePlayers[i] = true;
		}
		
		GetWorld()->ServerTravel(TravelGamePath);
	}
}

void ULab4GameInstance::Host() const
{
	if (!m_pSessionInterface.IsValid()) return;
	
	const FNamedOnlineSession *pOnlineSession = m_pSessionInterface->GetNamedSession(SessionNameConst);

	if (pOnlineSession != nullptr)
	{
		m_pSessionInterface->DestroySession(SessionNameConst);
	}
	else
	{
		CreateSession();
	}
}

void ULab4GameInstance::Join() const
{
	if (!m_pSessionInterface.IsValid()) return;
	if (!m_pSessionSearch.IsValid()) return;
	if (!m_pSessionSearch->SearchResults[m_JoinIndex].IsValid()) return;

	if (m_pMainMenu != nullptr)
	{
		m_pMainMenu->TeardownAll();
	}

	m_pSessionInterface->JoinSession(0, SessionNameConst, m_pSessionSearch->SearchResults[m_JoinIndex]);
}

void ULab4GameInstance::StartSession()
{
	if (m_pSessionInterface.IsValid())
	{
		m_pSessionInterface->StartSession(SessionNameConst);

		m_bIsInGame = true;
		
		for (int8 i = 0; i < m_Characters.Num(); ++i)
		{
			m_AlivePlayers[i] = true;
		}
	}
}

void ULab4GameInstance::StartMatch()
{
	if (m_Characters.Num() > 1)
	{
		StartSession();
		m_pLobby->StartGame();
	}
}

void ULab4GameInstance::RefreshServersList()
{
	m_pSessionSearch = MakeShareable(new FOnlineSessionSearch());

	if (m_pSessionSearch.IsValid())
	{
		m_pSessionSearch->bIsLanQuery = true; //
		m_pSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
				
		m_pSessionInterface->FindSessions(0,m_pSessionSearch.ToSharedRef());
	}
}

void ULab4GameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success) return;
	
	if (m_pMainMenu != nullptr)
	{
		m_pMainMenu->TeardownAll();
	}
	
	GetWorld()->ServerTravel(TravelLobbyPath);
}

void ULab4GameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (!Success) return;
	
	CreateSession();
}

void ULab4GameInstance::OnFindSessionsComplete(bool Success)
{
	if (Success && m_pSessionSearch.IsValid() && m_pMainMenu != nullptr)
	{
		TArray<FString> ServersList;
		
		for (const FOnlineSessionSearchResult& Session : m_pSessionSearch->SearchResults)
		{
			FString ServerName;
			Session.Session.SessionSettings.Get(ServerNameKey, ServerName);
			ServersList.Add(ServerName);
		}

		m_pMainMenu->OnInstanceFoundServers(ServersList);
	}
}

void ULab4GameInstance::OnJoinSessionComplete(FName Name, EOnJoinSessionCompleteResult::Type Result)
{
	if (!m_pSessionInterface.IsValid()) return;

	FString Address;
	if (!m_pSessionInterface->GetResolvedConnectString(Name, Address)) return;
	
	GetFirstLocalPlayerController()->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void ULab4GameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	m_bIsInGame = false;
	LoadMainMenu();
}

bool ULab4GameInstance::GetIsInGame() const
{
	return m_bIsInGame;
}

void ULab4GameInstance::CreateSession() const
{
	if (!m_pSessionInterface.IsValid()) return;
	
	FOnlineSessionSettings SessionSettings;
	
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		SessionSettings.bIsLANMatch = true;
	}
	else
	{
		SessionSettings.bIsLANMatch = false;
	}
	
	SessionSettings.NumPublicConnections = 10;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;

	FOnlineSessionSetting setting;
	setting.Data = m_ServerName.ToString();
	setting.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineServiceAndPing;
	
	SessionSettings.Set(ServerNameKey, setting);

	const FUniqueNetIdPtr NetID = GetFirstGamePlayer()->GetPreferredUniqueNetId().GetUniqueNetId();
	GetFirstGamePlayer()->SetCachedUniqueNetId(NetID);
	
	m_pSessionInterface->CreateSession(0, SessionNameConst, SessionSettings);
}

void ULab4GameInstance::LoadMainMenu() const
{
	GetFirstLocalPlayerController()->ClientTravel(TravelMainMenuPath, ETravelType::TRAVEL_Absolute);
}

int32 ULab4GameInstance::CalculateAlivePlayers() const
{
	int32 alive = 0;
	for (int i = 0; i < m_Characters.Num(); ++i)
	{
		if (m_AlivePlayers[i])
		{
			++alive;
		}
	}

	return alive;
}

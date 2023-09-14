#include "GameInitializer.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/GameStateBase.h"
#include "Lab4/Lab4Character.h"
#include "Lab4/GameInstances/Lab4GameInstance.h"
#include "Lab4/UserWidgets/Healthbar.h"
#include "Lab4/UserWidgets/LabelName.h"
#include "Lab4/UserWidgets/WidgetPlayers.h"
#include "Lab4/UserWidgets/WidgetStartMatch.h"
#include "Net/UnrealNetwork.h"

AGameInitializer::AGameInitializer()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	static ConstructorHelpers::FClassFinder<UUserWidget> LabelNameWBP(TEXT("/Game/MainMenu/WBP_LabelName"));
	m_LabelNameWidget = LabelNameWBP.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> HealthbarWBP(TEXT("/Game/MainMenu/WBP_Healthbar"));
	m_HealthbarWidget = HealthbarWBP.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> AllPlayersWBP(TEXT("/Game/MainMenu/WBP_AllPlayers"));
	m_AllPlayersWidget = AllPlayersWBP.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> StartMatchWBP(TEXT("/Game/MainMenu/WBP_StartMatch"));
	m_StartMatchWidget = StartMatchWBP.Class;
}

void AGameInitializer::BeginPlay()
{
	Super::BeginPlay();

	m_pLabelName = CreateWidget<ULabelName>(GetWorld(), m_LabelNameWidget);
	m_pLabelName->Setup(m_PlayerName);

	m_pHealthbar = CreateWidget<UHealthbar>(GetWorld(), m_HealthbarWidget);
	m_pHealthbar->Setup(1);
	
	m_pAllPlayers = CreateWidget<UWidgetPlayers>(GetWorld(), m_AllPlayersWidget);

	m_pStartMatch = CreateWidget<UWidgetStartMatch>(GetWorld(), m_StartMatchWidget);
}

void AGameInitializer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	TeardownAll();
}

void AGameInitializer::ConnectCharacter(ALab4Character* const pCharacter)
{
	m_pCharacter = pCharacter;

	if (m_pCharacter->IsServerInLobby())
	{
		m_pStartMatch->Setup();
	}
	else if (m_pStartMatch != nullptr)
	{
		m_pStartMatch->Teardown();
	}
}

void AGameInitializer::SetPlayerName(const FString& PlayerName)
{
	m_PlayerName = PlayerName;

	if (m_pLabelName != nullptr)
	{
		m_pLabelName->Setup(PlayerName);
	}
}

void AGameInitializer::SetHealth(const float HealthPercent) const
{
	m_pHealthbar->SetHealth(HealthPercent);
}

void AGameInitializer::CharacterOnShowPlayers(const TArray<FString>& PlayerNames) const
{
	m_pAllPlayers->Setup();
	m_pAllPlayers->SetPlayersList(PlayerNames);
}

void AGameInitializer::CharacterOnHidePlayers() const
{
	m_pAllPlayers->Teardown();
}

void AGameInitializer::CharacterStartMatch() const
{
	Cast<ULab4GameInstance>(GetGameInstance())->StartMatch();
}

void AGameInitializer::TeardownAll() const
{
	m_pAllPlayers->Teardown();
	m_pLabelName->Teardown();
	m_pHealthbar->Teardown();
	m_pStartMatch->Teardown();
}

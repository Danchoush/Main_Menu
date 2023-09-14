#include "MainMenuInitializer.h"

#include "Camera/CameraActor.h"
#include "InputSaver.h"
#include "JoinGameUpdater.h"
#include "Blueprint/UserWidget.h"
#include "Lab4/GameInstances/Lab4GameInstance.h"
#include "Lab4/UserWidgets/MainMenu.h"





AMainMenuInitializer::AMainMenuInitializer()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuWBP(TEXT("/Game/MainMenu/WBP_MainMenu_Video"));
	m_MainMenuWidget = MainMenuWBP.Class;
	
}

void AMainMenuInitializer::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	
	PlayerController->SetViewTargetWithBlend(Camera_3, 0, EViewTargetBlendFunction::VTBlend_Linear);
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(MyShake, 1.0f);
	


	m_pGameInstance = GetGameInstance<ULab4GameInstance>();
	m_pGameInstance->ConnectMainMenuInitializer(this);

	m_pMainMenu = CreateWidget<UMainMenu>(m_pGameInstance, m_MainMenuWidget);
	m_pMainMenu->SetMainMenu(this);
	m_pMainMenu->Setup();

	UWorld* const pWorld = GetWorld();

	m_pInputSaver = pWorld->SpawnActor<AInputSaver>(AInputSaver::StaticClass());

	const FString ServerName = m_pInputSaver->LoadServerName();
	const FString PlayerName = m_pInputSaver->LoadPlayerName();

	m_pMainMenu->SetServerName(ServerName);
	m_pMainMenu->SetPlayerName(PlayerName);

	m_pGameInstance->SetServerName(ServerName);
	m_pGameInstance->SetPlayerName(PlayerName);

	m_pServersListUpdater = pWorld->SpawnActor<AJoinGameUpdater>(AJoinGameUpdater::StaticClass());
	m_pServersListUpdater->SetMainMenu(this);
}

void AMainMenuInitializer::OnWidgetToJoin() const
{

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->SetViewTargetWithBlend(Camera_2, 0, EViewTargetBlendFunction::VTBlend_Linear);
	
	m_pGameInstance->RefreshServersList();
}

void AMainMenuInitializer::OnWidgetToCreate() const
{

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->SetViewTargetWithBlend(Camera_4, 0, EViewTargetBlendFunction::VTBlend_Linear);
	
}

void AMainMenuInitializer::OnWidgetToMain(bool bIsFromCreate) const
{

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->SetViewTargetWithBlend(Camera_3, 0, EViewTargetBlendFunction::VTBlend_Linear);

	if (!bIsFromCreate)
	{
		m_pServersListUpdater->StopTimer();
	}
}

void AMainMenuInitializer::OnWidgetToPlayerNameAtJoining(uint32 Index) const
{

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->SetViewTargetWithBlend(Camera_2, 0, EViewTargetBlendFunction::VTBlend_Linear);
	
	m_pServersListUpdater->StopTimer();
	m_pGameInstance->SetJoinIndex(Index);
}

void AMainMenuInitializer::OnWidgetToPlayerNameAtCreation(const FString& ServerName) const
{

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->SetViewTargetWithBlend(Camera_1, 0, EViewTargetBlendFunction::VTBlend_Linear);
	
	m_pGameInstance->SetServerName(ServerName);
	m_pInputSaver->SaveServerName(ServerName);
}

void AMainMenuInitializer::OnWidgetToStartGame(const FString& PlayerName, const bool bIsFromCreate) const
{
	m_pInputSaver->SavePlayerName(PlayerName);
	m_pGameInstance->SetPlayerName(PlayerName);

	if (bIsFromCreate)
	{
		m_pGameInstance->Host();
	}
	else
	{
		m_pGameInstance->Join();
	}
}

void AMainMenuInitializer::OnInstanceFoundServers(const TArray<FString>& ServerNames) const
{
	m_pMainMenu->SetServersList(ServerNames);

	if (ServerNames.Num() == 0)
	{
		m_pGameInstance->RefreshServersList();
	}
	else
	{
		m_pServersListUpdater->BeginTimer(m_UpdateServersListTime);
	}
}

void AMainMenuInitializer::OnUpdaterTimerRunOut() const
{
	m_pGameInstance->RefreshServersList();
}

void AMainMenuInitializer::TeardownAll() const
{
	m_pMainMenu->Teardown();
	m_pServersListUpdater->StopTimer();
}

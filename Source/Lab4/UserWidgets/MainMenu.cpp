#include "MainMenu.h"
#include "UObject/ConstructorHelpers.h"
#include "ServerRow.h"
#include "Components/EditableText.h"
#include "Components/WidgetSwitcher.h"
#include "Lab4/Actors/MainMenuInitializer.h"


UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ServerRowWBP(
		TEXT("/Game/MainMenu/WBP_ServerRow"));

	m_ServerRowWidget = ServerRowWBP.Class;
}

bool UMainMenu::Initialize()
{
	if (!Super::Initialize()) return false;



	CreateGameButton->OnClicked.AddDynamic(this, &UMainMenu::OnClickedCreate);
	JoinGameButton->OnClicked.AddDynamic(this, &UMainMenu::OnClickedJoin);
	CancelJoinConfirmButton->OnClicked.AddDynamic(this, &UMainMenu::OnClickedMain);
	JoinGameConfirmButton->OnClicked.AddDynamic(this, &UMainMenu::OnClickedPlayerNameAtJoining);
	CancelSessionNameConfirmButton->OnClicked.AddDynamic(this, &UMainMenu::OnClickedMain);
	SessionNameConfirmButton->OnClicked.AddDynamic(this, &UMainMenu::OnClickedPlayerNameAtCreation);
	CancelNameConfirmButton->OnClicked.AddDynamic(this, &UMainMenu::OnClickedMain);
	NameConfirmButton->OnClicked.AddDynamic(this, &UMainMenu::OnClickedStartGame);
	return true;
}

void UMainMenu::SetMainMenu(AMainMenuInitializer* const pMainMenu)
{
	m_pMainMenu = pMainMenu;
}

void UMainMenu::SetServersList(const TArray<FString>& List)
{
	ServersList->ClearChildren();
	m_SelectedIndex.Reset();
	
	UWorld *pWorld = GetWorld();

	if (List.Num() == 0)
	{
		UServerRow *pServerRow = CreateWidget<UServerRow>(GetWorld(), m_ServerRowWidget);
		pServerRow->SetServerName(Nothing);
		ServersList->AddChild(pServerRow);
	}
	else
	{
		uint32 i = 0;
		for (const FString& Row : List)
		{
			UServerRow *pServerRow = CreateWidget<UServerRow>(pWorld, m_ServerRowWidget);
			pServerRow->SetServerName(Row);
			pServerRow->Setup(this, i);
			ServersList->AddChild(pServerRow);

			++i;
		}
	}
}

void UMainMenu::SetServerName(const FString& SessionName) const
{
	SessionNameEnterText->SetText(FText::FromString(SessionName));
}

void UMainMenu::SetPlayerName(const FString& Name) const
{
	NameEnterText->SetText(FText::FromString(Name));
}

void UMainMenu::OnClickedCreate()
{
	m_bIsCreateGame = true;
	MenuSwitcher->SetActiveWidget(SessionNameMenu);
	
	m_pMainMenu->OnWidgetToCreate();
}

void UMainMenu::OnClickedJoin()
{
	m_bIsCreateGame = false;
	m_SelectedIndex.Reset();
	MenuSwitcher->SetActiveWidget(JoinMenu);

	ServersList->ClearChildren();
	UServerRow *pServerRow = CreateWidget<UServerRow>(GetWorld(), m_ServerRowWidget);
	pServerRow->SetServerName(Waiting);
	ServersList->AddChild(pServerRow);

	m_pMainMenu->OnWidgetToJoin();
}

void UMainMenu::OnClickedMain()
{
	MenuSwitcher->SetActiveWidget(MainMenu);

	m_pMainMenu->OnWidgetToMain(m_bIsCreateGame);
}

void UMainMenu::OnClickedPlayerNameAtJoining()
{
	if (!m_SelectedIndex.IsSet()) return;
	
	MenuSwitcher->SetActiveWidget(NameMenu);

	m_pMainMenu->OnWidgetToPlayerNameAtJoining(m_SelectedIndex.GetValue());
}

void UMainMenu::OnClickedPlayerNameAtCreation()
{
	if (SessionNameEnterText->GetText().IsEmpty()) return;

	MenuSwitcher->SetActiveWidget(NameMenu);

	m_pMainMenu->OnWidgetToPlayerNameAtCreation(SessionNameEnterText->GetText().ToString());
}

void UMainMenu::OnClickedStartGame()
{
	const FString EnteredPlayerName = NameEnterText->GetText().ToString();
	
	if (EnteredPlayerName.IsEmpty()) return;

	m_pMainMenu->OnWidgetToStartGame(EnteredPlayerName, m_bIsCreateGame);
}

void UMainMenu::Setup()
{
	this->AddToViewport();

	APlayerController *pPlayerController = GetWorld()->GetFirstPlayerController();

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(this->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	pPlayerController->SetInputMode(InputModeData);
	pPlayerController->bShowMouseCursor = true;
}

void UMainMenu::Teardown()
{
	this->RemoveFromViewport();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	const FInputModeGameOnly InputModeData;

	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = false;
}

void UMainMenu::SelectIndex(uint32 Index)
{
	UpdateChildren(Index);
	m_SelectedIndex = Index;
}

void UMainMenu::UpdateChildren(uint32 NewIndexSelected)
{
	if (m_SelectedIndex.IsSet())
	{
		UServerRow *pRow = Cast<UServerRow>(ServersList->GetChildAt(m_SelectedIndex.GetValue()));
		if (pRow != nullptr)
		{
			pRow->bIsSelected = false;
		}
	}

	UServerRow *pRow = Cast<UServerRow>(ServersList->GetChildAt(NewIndexSelected));
	if (pRow != nullptr)
	{
		pRow->bIsSelected = true;
	}
}

#include "LoadingInitializer.h"

#include "Blueprint/UserWidget.h"
#include "Lab4/GameInstances/Lab4GameInstance.h"

ALoadingInitializer::ALoadingInitializer()
{
 	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	static ConstructorHelpers::FClassFinder<UUserWidget> LoadingWBP(TEXT("/Game/Loading/WBP_LoadingBar"));
	m_LoadingWidget = LoadingWBP.Class;
}

void ALoadingInitializer::BeginPlay()
{
	Super::BeginPlay();
	
	m_pLoadingBar = CreateWidget<ULoadingBar>(GetGameInstance<ULab4GameInstance>(), m_LoadingWidget);
	m_pLoadingBar->Setup();
}

void ALoadingInitializer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	m_pLoadingBar->Teardown();
}

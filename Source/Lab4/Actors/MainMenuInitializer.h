#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "GameFramework/Actor.h"
#include "RandomCameraShake.h"
#include "MainMenuInitializer.generated.h"

UCLASS()
class LAB4_API AMainMenuInitializer : public AActor
{
	GENERATED_BODY()

public:
	AMainMenuInitializer();
	
	void OnWidgetToJoin() const;
	void OnWidgetToCreate() const;
	void OnWidgetToMain(bool bIsFromCreate) const;
	void OnWidgetToPlayerNameAtJoining(uint32 Index) const;
	void OnWidgetToPlayerNameAtCreation(const FString& ServerName) const;
	void OnWidgetToStartGame(const FString& PlayerName, const bool bIsFromCreate) const;
	
	void OnInstanceFoundServers(const TArray<FString>& ServerNames) const;

	void OnUpdaterTimerRunOut() const;
	
	void TeardownAll() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UMatineeCameraShake> MyShake;


	UPROPERTY(EditAnywhere)
		ACameraActor* Camera_1;
	UPROPERTY(EditAnywhere)
		ACameraActor* Camera_2;
	UPROPERTY(EditAnywhere)
		ACameraActor* Camera_3;
	UPROPERTY(EditAnywhere)
		ACameraActor* Camera_4;
	UPROPERTY(EditAnywhere)
		ACameraActor* Camera_5;

private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"))
	float m_UpdateServersListTime;

	UPROPERTY()
	class ULab4GameInstance *m_pGameInstance;
	
	UPROPERTY()
	class UMainMenu *m_pMainMenu;

	UPROPERTY()
	class AInputSaver *m_pInputSaver;
	
	UPROPERTY()
	class AJoinGameUpdater *m_pServersListUpdater;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> m_MainMenuWidget;
};

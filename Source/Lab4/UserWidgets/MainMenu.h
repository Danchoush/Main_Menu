#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"





UCLASS()
class LAB4_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UMainMenu(const FObjectInitializer& ObjectInitializer);
	
	void SetMainMenu(class AMainMenuInitializer* const pMainMenu);
	void SetServersList(const TArray<FString>& List);

	void SetServerName(const FString& SessionName) const;
	void SetPlayerName(const FString& Name) const;

	
	UFUNCTION()
	void OnClickedCreate();
	UFUNCTION()
	void OnClickedJoin();
	UFUNCTION()
	void OnClickedMain();
	UFUNCTION()
	void OnClickedPlayerNameAtJoining();
	UFUNCTION()
	void OnClickedPlayerNameAtCreation();
	UFUNCTION()
	void OnClickedStartGame();
	
	void Setup();
	void Teardown();

	void SelectIndex(uint32 Index);

protected:
	virtual bool Initialize() override;

	
private:
	void UpdateChildren(uint32 NewIndexSelected);
	
	UPROPERTY(meta=(BindWidget))
	class UButton *CreateGameButton;

	UPROPERTY(meta=(BindWidget))
	class UButton *JoinGameButton;

	UPROPERTY(meta=(BindWidget))
	class UButton *CancelJoinConfirmButton;

	UPROPERTY(meta=(BindWidget))
	class UButton *JoinGameConfirmButton;

	UPROPERTY(meta=(BindWidget))
	class UButton *CancelSessionNameConfirmButton;

	UPROPERTY(meta=(BindWidget))
	class UButton *SessionNameConfirmButton;

	UPROPERTY(meta=(BindWidget))
	class UButton *CancelNameConfirmButton;

	UPROPERTY(meta=(BindWidget))
	class UButton *NameConfirmButton;

	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher *MenuSwitcher;

	UPROPERTY(meta=(BindWidget))
	class UWidget *MainMenu;

	UPROPERTY(meta=(BindWidget))
	class UWidget *JoinMenu;

	UPROPERTY(meta=(BindWidget))
	class UWidget *SessionNameMenu;
	
	UPROPERTY(meta=(BindWidget))
	class UWidget *NameMenu;

	UPROPERTY(meta=(BindWidget))
	class UPanelWidget *ServersList;

	UPROPERTY(meta=(BindWidget))
	class UEditableText *SessionNameEnterText;
	
	UPROPERTY(meta=(BindWidget))
	class UEditableText *NameEnterText;
	
	UPROPERTY()
	class AMainMenuInitializer *m_pMainMenu;

	TSubclassOf<UUserWidget> m_ServerRowWidget;

	TOptional<uint32> m_SelectedIndex;
	
	bool m_bIsCreateGame;

	const FString Waiting = "Please, wait...";
	const FString Nothing = "Sorry, nothing has found. Try again...";
};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameInitializer.generated.h"

UCLASS()
class LAB4_API AGameInitializer : public AActor
{
	GENERATED_BODY()

public:
	AGameInitializer();

	void ConnectCharacter(class ALab4Character* const pCharacter);

	void SetPlayerName(const FString& PlayerName);

	void SetHealth(const float HealthPercent) const;
	
	void CharacterOnShowPlayers(const TArray<FString>& PlayerNames) const;
	void CharacterOnHidePlayers() const;

	void CharacterStartMatch() const;
	
	void TeardownAll() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"))
	float m_TimePingRecheck;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"))
	float m_TimerExit;

	UPROPERTY()
	class ALab4Character *m_pCharacter;

	UPROPERTY()
	class UWidgetPlayers *m_pAllPlayers;
	
	UPROPERTY()
	class ULabelName *m_pLabelName;

	UPROPERTY()
	class UHealthbar *m_pHealthbar;

	UPROPERTY()
	class UWidgetStartMatch *m_pStartMatch;
	
	TSubclassOf<UUserWidget> m_AllPlayersWidget;
	TSubclassOf<UUserWidget> m_LabelNameWidget;
	TSubclassOf<UUserWidget> m_HealthbarWidget;
	TSubclassOf<UUserWidget> m_StartMatchWidget;

	FString m_PlayerName;
};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Lab4/UserWidgets/LoadingBar.h"
#include "LoadingInitializer.generated.h"

UCLASS()
class LAB4_API ALoadingInitializer : public AActor
{
	GENERATED_BODY()
	
public:
	ALoadingInitializer();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	ULoadingBar *m_pLoadingBar;
	
	TSubclassOf<UUserWidget> m_LoadingWidget;
};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingBar.generated.h"

UCLASS()
class LAB4_API ULoadingBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup();
	void Teardown();
};

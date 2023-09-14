#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetStartMatch.generated.h"

UCLASS()
class LAB4_API UWidgetStartMatch : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Setup();
	void Teardown();

};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Healthbar.generated.h"

UCLASS()
class LAB4_API UHealthbar : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(const float HealthPercent);
	void Teardown();

	void SetHealth(const float HealthPercent) const;
	
private:
	UPROPERTY(meta=(BindWidget))
	class UProgressBar *HealthBar;
};

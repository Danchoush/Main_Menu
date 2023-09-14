#include "Healthbar.h"

#include "Components/ProgressBar.h"

void UHealthbar::Setup(const float HealthPercent)
{
	this->AddToViewport();

	HealthBar->SetPercent(HealthPercent);
}

void UHealthbar::Teardown()
{
	this->RemoveFromViewport();
}

void UHealthbar::SetHealth(const float HealthPercent) const
{
	HealthBar->SetPercent(HealthPercent);
}

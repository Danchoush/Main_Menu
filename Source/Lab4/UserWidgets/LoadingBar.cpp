#include "LoadingBar.h"

void ULoadingBar::Setup()
{
	this->AddToViewport();
}

void ULoadingBar::Teardown()
{
	this->RemoveFromViewport();
}

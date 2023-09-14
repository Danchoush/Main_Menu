#include "WidgetStartMatch.h"

void UWidgetStartMatch::Setup()
{
	this->AddToViewport();
}

void UWidgetStartMatch::Teardown()
{
	this->RemoveFromViewport();
}
#include "EldenRing_Mod/Controller/EldenTitleController.h"
#include "EldenRing_Mod/Widget/EldenTitleWidget.h"
#include "Blueprint/UserWidget.h"

void AEldenTitleController::BeginPlay()
{
	Super::BeginPlay();

	if (!WidgetClass) return;

	CurrentWidget = CreateWidget<UEldenTitleWidget>(this, WidgetClass);
	if (!CurrentWidget) return;
	CurrentWidget->AddToViewport();

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(CurrentWidget->TakeWidget());
	SetInputMode(InputMode);
	bShowMouseCursor = true;


}

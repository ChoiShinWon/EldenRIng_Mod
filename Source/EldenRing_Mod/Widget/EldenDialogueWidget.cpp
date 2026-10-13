
#include "EldenRing_Mod/Widget/EldenDialogueWidget.h"
#include "Components/TextBlock.h"

void UEldenDialogueWidget::SetLine(const FText& NPCName, const FText& Line)
{
	Txt_Speaker->SetText(NPCName);
	Txt_Line->SetText(Line);
}

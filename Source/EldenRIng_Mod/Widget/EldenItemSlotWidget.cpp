#include "EldenRing_Mod/Widget/EldenItemSlotWidget.h"
#include "EldenRing_Mod/Item/EldenItemDefinition.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UEldenItemSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ItemClickButton->OnClicked.AddDynamic(this, &UEldenItemSlotWidget::OnSlotClicked);
}

void UEldenItemSlotWidget::SetItemData(UEldenItemDefinition* Def, int32 Count, int32 InSlotIndex)
{
	DisplayedItem = Def;
	SlotIndex = InSlotIndex;

	if (!Def)
	{
		ItemImage->SetBrushFromTexture(nullptr);
		ItemCountText->SetText(FText::GetEmpty());
	}
	else
	{
		ItemImage->SetBrushFromTexture(Def->Icon);
		ItemCountText->SetText(FText::AsNumber(Count));
	}
	
}

void UEldenItemSlotWidget::OnSlotClicked()
{

	OnSelectedItemClicked.Broadcast(DisplayedItem, SlotIndex);

}


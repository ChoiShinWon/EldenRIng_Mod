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
		ItemImage->SetColorAndOpacity(FLinearColor(1, 1, 1, 0));
		ItemCountText->SetText(FText::GetEmpty());
		ItemNameText->SetText(FText::GetEmpty());
	}
	else
	{
		ItemImage->SetBrushFromTexture(Def->Icon);
		ItemImage->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));
		if (Count > 0)
		{
			ItemCountText->SetText(FText::AsNumber(Count));
		}
		else
		{
			ItemCountText->SetText(FText::GetEmpty());
		}

		ItemNameText->SetText(Def->DisplayName);
	}
	
}

void UEldenItemSlotWidget::OnSlotClicked()
{

	OnSelectedItemClicked.Broadcast(DisplayedItem, SlotIndex);

}


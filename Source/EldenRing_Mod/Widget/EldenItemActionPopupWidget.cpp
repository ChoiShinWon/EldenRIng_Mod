#include "EldenRing_Mod/Widget/EldenItemActionPopupWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UEldenItemActionPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ActionButton->OnClicked.AddDynamic(this, &UEldenItemActionPopupWidget::ButtonClickedEvent);

}

void UEldenItemActionPopupWidget::SetPopup(UEldenItemDefinition* Item, EItemActionType ActionType)
{
	if (!Item || ActionType == EItemActionType::None) return;
	PopupItem = Item;
	PopupMode = ActionType;

	if (ActionType == EItemActionType::Equip)
	{
		ActionText->SetText(FText::FromString(TEXT("아이템 장착")));
	}
	else if (ActionType == EItemActionType::UnEquip)
	{
		ActionText->SetText(FText::FromString(TEXT("아이템 해제")));
	}
	else if (ActionType == EItemActionType::Use)
	{
		ActionText->SetText(FText::FromString(TEXT("사용하기")));
	}
}


void UEldenItemActionPopupWidget::ButtonClickedEvent()
{
	OnActionConfirmed.ExecuteIfBound(PopupItem, PopupMode);
}



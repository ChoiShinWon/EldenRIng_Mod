#include "EldenRing_Mod/Widget/EldenItemSlotWidget.h"
#include "EldenRing_Mod/Item/EldenItemDefinition.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UEldenItemSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼이 눌리면 OnSlotClicked 함수를 호출하도록 연결
	ItemClickButton->OnClicked.AddDynamic(this, &UEldenItemSlotWidget::OnSlotClicked);
}

void UEldenItemSlotWidget::SetItemData(UEldenItemDefinition* Def, int32 Count, int32 InSlotIndex)
{
	DisplayedItem = Def;
	SlotIndex = InSlotIndex;

	if (!Def)
	{
		// 빈 슬롯 표현: 아이콘 nullptr, 투명 처리, 텍스트는 공백
		ItemImage->SetBrushFromTexture(nullptr);
		ItemImage->SetColorAndOpacity(FLinearColor(1, 1, 1, 0));
		ItemCountText->SetText(FText::GetEmpty());
		ItemNameText->SetText(FText::GetEmpty());
	}
	else
	{
		// 아이템이 있는 슬롯: 아이콘 표시, 불투명 
		ItemImage->SetBrushFromTexture(Def->Icon);
		ItemImage->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));

		// 개수가 의미 있을 때만 숫자 표시
		if (Def->MaxCount > 1)
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
	// 지금 들고 있는 데이터 (DisplayedItem)와 내 위치(SlotIndex)를
	// 그대로 실어서 상위에 통지
	// 빈 슬롯이어도 그냥 브로드캐스트 -> 상위에서 nullptr 체크로 거를지 말지 결정
	OnSelectedItemClicked.Broadcast(DisplayedItem, SlotIndex);

}



#include "EldenRing_Mod/Widget/EldenItemPickupPopupWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"
#include "EldenRing_Mod/Item/EldenItemDefinition.h"

void UEldenItemPickupPopupWidget::ShowPopup(const UEldenItemDefinition* Def, int32 Amount, bool bIsNew)
{
	if (!Def || !Anim_Show) return;

	Img_Icon->SetBrushFromTexture(Def->Icon);
	Txt_ItemName->SetText(Def->DisplayName);
	if (Txt_Amount)
	{
		if (Amount > 1)
		{
			Txt_Amount->SetText(FText::Format(FText::FromString(TEXT("x{0}")), FText::AsNumber(Amount)));
			Txt_Amount->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Txt_Amount->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	Txt_NewTag->SetVisibility(bIsNew ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	PlayAnimation(Anim_Show);
	if (AcquireSound)
	{
		UGameplayStatics::PlaySound2D(this, AcquireSound);
	}

	bShowing = true;
}

void UEldenItemPickupPopupWidget::HidePopup()
{
	if (!bShowing) return;

	bShowing = false;
	PlayAnimationReverse(Anim_Show);
}

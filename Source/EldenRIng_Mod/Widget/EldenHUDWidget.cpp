#include "EldenHUDWidget.h"
#include "Components/ProgressBar.h"
#include "EldenRing_Mod/Character/EldenCharacter.h" 
#include "Kismet/KismetMathLibrary.h"

void UEldenHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (PlayerRef == nullptr)
	{
		PlayerRef = Cast<AEldenCharacter>(GetOwningPlayerPawn());
	}

	if (PlayerRef && StaminaBar && GhostBar)
	{
		// Getter 함수 사용 
		float TargetPercent = PlayerRef->GetCurrentStamina() / PlayerRef->GetMaxStamina();

		StaminaBar->SetPercent(TargetPercent);

		// 노란색 바 보간
		GhostPercent = UKismetMathLibrary::FInterpTo(GhostPercent, TargetPercent, InDeltaTime, 2.0f);
		GhostBar->SetPercent(GhostPercent);
	}
}
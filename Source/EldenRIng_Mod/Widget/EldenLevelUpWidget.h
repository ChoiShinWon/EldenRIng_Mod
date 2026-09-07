// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EldenLevelUpWidget.generated.h"

class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class ELDENRING_MOD_API UEldenLevelUpWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// 생명력 버튼 및 텍스트
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_VigorMinus;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_VigorPlus;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_VigorValue;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_OriginalVigor;
		

	// 지구력 버튼 및 텍스트
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_EnduranceMinus;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_EndurancePlus;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_EnduranceValue;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_OriginalEndurance;

	// 공격력 버튼 및 텍스트
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_StrengthMinus;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_StrengthPlus;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_StrengthValue;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_OriginalStrength;

	// 최종 확인 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Confirm;

	// 취소, 나가기 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Cancel;

	// 남은 룬을 표시할 텍스트 블록
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_LevelUpRunes;

	// 남은 룬을 계산해서 텍스트를 갱신해줄 함수
	void UpdateRuneText();
	

	// 임시 데이터 및 참조 변수 선언
	int32 PendingVigor;
	int32 PendingEndurance;
	int32 PendingStrength;
	int32 TotalPendingRuneCost;

	UPROPERTY()
	class AEldenCharacter* PlayerCharacter;

	UPROPERTY()
	class UEldenStatComponent* PlayerStatComponent;

	UFUNCTION()
	void OnVigorPlusClicked();
	
	UFUNCTION()
	void OnVigorMinusClicked();

	UFUNCTION()
	void OnEndurancePlusClicked();

	UFUNCTION()
	void OnEnduranceMinusClicked();

	UFUNCTION()
	void OnStrengthPlusClicked();

	UFUNCTION()
	void OnStrengthMinusClicked();

	UFUNCTION()
	void OnConfirmClicked();

	UFUNCTION()
	void OnCancelClicked();
	
};

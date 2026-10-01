#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EldenItemSlotWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;
class UEldenItemDefinition;

// 슬롯 하나가 클릭됐을 때 어떤 아이템을 몇 번 슬롯에서 클릭했는지
// 상위 (인벤토리/장비 위젯)에 알리는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemClickedDelegate, UEldenItemDefinition*, ItemDef, int32, SlotIndex);

// 그리드 한 칸에 들어가는 재사용 가능한 작은 위젯
// 인벤토리 탭/장비 탭이 둘 다 이 위젯을 N개 찍어내서 그리드에 채워 넣음
UCLASS()
class ELDENRING_MOD_API UEldenItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// 버튼 클릭 델리게이트 구독을 여기서 한 번만 건다 (위젯 생성 시점)
	virtual void NativeConstruct() override;

	// BindWidget 디자이너탭에 같은 이름의 위젯이 반드시 있어야함
	// 있으면 자동으로 포인터 채워진다.
	UPROPERTY(meta = (BindWidget))
	class UImage* ItemImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemCountText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemNameText;

	UPROPERTY(meta = (BindWidget))
	class UButton* ItemClickButton;

	// 지금 이 슬롯이 표시하고 있는 아이템 데이터 (비어있는 슬롯이면 null)
	UPROPERTY()
	class UEldenItemDefinition* DisplayedItem;

	// 버튼의 OnClicked와 연결될 실제 핸들러
	// UFUNCTION()을 붙여아 AddDynmaic에 바인딩 가능
	UFUNCTION()
	void OnSlotClicked();

	// 이 슬롯이 그리드에서 몇 번째 칸인지 (클릭 시 상위에 알려주기 위함)
	int32 SlotIndex = -1;

public:
	// 상위 위젯이 그리드를 새로 그릴 때마다 호출해서
	// 이 슬롯의 내용을 갱신 시키는 함수
	void SetItemData(UEldenItemDefinition* Def, int32 Count, int32 InSlotIndex);

	// 상위 위젯이 구독해서 이 슬롯이 클릭됐다는 이벤트를 받는 델리게이트
	UPROPERTY()
	FOnItemClickedDelegate OnSelectedItemClicked;
};

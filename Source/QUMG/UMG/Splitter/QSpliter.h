// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Runtime/UMG/Public/Components/PanelWidget.h"
//#include "UMG/Splitter/CppWgt_BaseSplitter.h"
#include "QSpliter.generated.h"



UENUM(BlueprintType)
enum class EM_SplitterDirectionType : uint8
{
	EM_Vertical			UMETA( DisplayName = "Vertical"		) ,
	EM_Horizontal		UMETA( DisplayName = "Horizontal"	)
};


UENUM(BlueprintType)
enum class EM_SplitterResizeMode : uint8
{
	EM_FixedPosition	UMETA( DisplayName = "FixedPosition"	) ,
	EM_FixedSize		UMETA( DisplayName = "FixedSize"		) ,
	EM_Fill				UMETA( DisplayName = "Fill"				)
};


class  SSplitterEx;

/**
 *
 */
UCLASS()
class QUMG_API UQSpliter : public UPanelWidget
{
	GENERATED_BODY()
public:

#if WITH_EDITOR
	// UWidget interface
	virtual const FText GetPaletteCategory() override;

	// End UWidget interface
#endif

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
public:
#pragma region Event
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSplitteResize, UWidget*, wgt, float, val);
	UPROPERTY(BlueprintAssignable, Category = "Spliter | Event")
		FOnSplitteResize OnSplitteItemResize;

#pragma endregion Event

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Variant")
		float HandSize = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Variant")
		EM_SplitterDirectionType Direction = EM_SplitterDirectionType::EM_Horizontal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Variant")
		EM_SplitterResizeMode ResizeMode = EM_SplitterResizeMode::EM_FixedPosition;

public:
	UFUNCTION(BlueprintCallable, Category = "Spliter | Function")
		///获取未收起隐藏的Children数量
		int32 GetChildrenCountByNotCollapsed() const;

	UFUNCTION(BlueprintCallable, Category = "Spliter | Function")
		/// 获取系数总和计数
		float GetCoefficientCountByNotCollapsed() const;
private:
	EOrientation GetEOrientation() const;
	ESplitterResizeMode::Type GetESplitterResizeMode() const;
protected:  

	// UPanelWidget
	virtual UClass* GetSlotClass() const override;
	virtual void OnSlotAdded(UPanelSlot* slot) override;
	virtual void OnSlotRemoved(UPanelSlot* slot) override;
	// End UPanelWidget
protected:
	void Handle_OnSplitteItemResize(UWidget* wgt, float val);

protected:
	TSharedPtr<class SSplitterEx> MySplitter;

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface
};



/** Splitter extend */
class QUMG_API SSplitterEx : public SSplitter
{
public:
	virtual int32 OnPaint(const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;
};


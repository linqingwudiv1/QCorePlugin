// Fill out your copyright notice in the Description page of Project Settings.

#include "CppWgt_BaseSplitter.h"

#include "Slate.h"
#include "SConstraintCanvas.h"

#include "Kismet/GameplayStatics.h"

void SMySlateWidget::Construct(const FArguments& InArgs)
{
	TSharedRef<SBorder> border = SNew(SBorder);
	border->SetBorderBackgroundColor(FLinearColor::Red);
	border->SetForegroundColor(FLinearColor(0, 255, 0, 0.5));
	border->SetBorderImage(&brush);
	border->SetColorAndOpacity(FLinearColor::Green);

	TSharedRef<SBorder> border1 = SNew(SBorder);
	border1->SetBorderBackgroundColor(FLinearColor::Green);
	border1->SetForegroundColor(FLinearColor(0, 255, 0, 0.5));
	border1->SetBorderImage(&brush);
	border1->SetColorAndOpacity(FLinearColor::Green);

	TSharedRef<SBorder> border2 = SNew(SBorder);
	border2->SetBorderBackgroundColor(FLinearColor::Blue);
	border2->SetForegroundColor(FLinearColor(0, 255, 0, 0.5));
	border2->SetBorderImage(&brush);
	border2->SetColorAndOpacity(FLinearColor::Green);

	auto container = SNew(SVerticalBox) + SVerticalBox::Slot().FillHeight(1)
		[
			SNew(SSplitter)
			+ SSplitter::Slot()
			.Value(0.75f)
			[
				border1
			]
			+ SSplitter::Slot()
			.Value(0.25f)
			[
				border2
			]
		];

	SConstraintCanvas::FSlot &temp_slot = SConstraintCanvas::Slot();
	temp_slot.Anchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f))
		.Offset(FMargin(100.0f, 100.0f, 100.0f, 100.0f))
		.ZOrder(1)
		.AttachWidget(container);

	SUserWidget::Construct(
		SUserWidget::FArguments()
		[
			SNew(SConstraintCanvas) + temp_slot
		]
	);
}

TSharedRef<SMySlateWidget> SMySlateWidget::New()
{
	return MakeShareable(new SMySlateWidget());
}

#if WITH_EDITOR
const FText UCppWgt_BaseSplitter::GetPaletteCategory()
{
	return NSLOCTEXT("UContenSlatetWidget", "MyCustomSlate", "CustomSlate");
}
#endif

TSharedRef<SWidget> UCppWgt_BaseSplitter::RebuildWidget()
{
	//auto temporary_wgt = CreateWidget(this, ins->ColorPickerUMGClass);

	TSharedRef<SWidget> border1 = SNew(SBorder);

	//temporary_wgt->TakeWidget();

	auto container = SNew(SVerticalBox) + SVerticalBox::Slot().FillHeight(1)
		[
			SNew(SSplitter)
			+ SSplitter::Slot()
			.Value(0.75f)
			[
				border1
			]
			+ SSplitter::Slot()
			.Value(0.25f)
			[
				SNew(SMySlateWidget)
			]
		];

	SConstraintCanvas::FSlot &temp_slot = SConstraintCanvas::Slot();
	temp_slot.Anchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f))
		.Offset(FMargin(100.0f, 100.0f, 100.0f, 100.0f))
		.ZOrder(1)
		.AttachWidget(container);

	auto ret_wgt = SNew(SConstraintCanvas) + temp_slot;
	return  ret_wgt;
}

int32 SSplitterEx::OnPaint(	const FPaintArgs & Args	, 
							const FGeometry & AllottedGeometry	, 
							const FSlateRect & MyCullingRect	, 
							FSlateWindowElementList & OutDrawElements	, 
							int32 LayerId	, 
							const FWidgetStyle & InWidgetStyle	, 
							bool bParentEnabled						) const
{

	FArrangedChildren ArrangedChildren(EVisibility::Visible);
	ArrangeChildren(AllottedGeometry, ArrangedChildren);

	int32 MaxLayerId = PaintArrangedChildren(Args, ArrangedChildren, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	const FSlateBrush* NormalHandleBrush = &Style->HandleNormalBrush;

	// Draw the splitter above any children
	MaxLayerId += 1;

	///绘制分割线Make Box
	for (int32 ChildIndex = 0; ChildIndex < ArrangedChildren.Num(); ++ChildIndex)
	{
		//if (ChildIndex == 1) { continue; }
		const FGeometry& GeometryAfterSplitter = ArrangedChildren[FMath::Clamp(ChildIndex + 1, 0, ArrangedChildren.Num() - 1)].Geometry;

		const float HalfHitDetectionSplitterHandleSize = FMath::RoundToInt(HitDetectionSplitterHandleSize / 2.0f);
		const float HalfPhysicalSplitterHandleSize = PhysicalSplitterHandleSize;

		FVector2D HandleSize;
		FVector2D HandlePosition;

		if (Orientation == Orient_Horizontal)
		{
			HandleSize.Set(PhysicalSplitterHandleSize, GeometryAfterSplitter.Size.Y);
			HandlePosition.Set(-PhysicalSplitterHandleSize, 0);
		}
		else
		{
			HandleSize.Set(GeometryAfterSplitter.Size.X, PhysicalSplitterHandleSize);
			HandlePosition.Set(0, -PhysicalSplitterHandleSize);
		}

		if (HoveredHandleIndex != ChildIndex)
		{
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				MaxLayerId,
				GeometryAfterSplitter.ToPaintGeometry(HandlePosition, HandleSize, 1.0f),
				NormalHandleBrush,
				ShouldBeEnabled(bParentEnabled) ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect,
				InWidgetStyle.GetColorAndOpacityTint() * NormalHandleBrush->TintColor.GetSpecifiedColor()
			);
		}
		else
		{
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				MaxLayerId,
				GeometryAfterSplitter.ToPaintGeometry(HandlePosition, HandleSize, 1.0f),
				&Style->HandleHighlightBrush,
				ShouldBeEnabled(bParentEnabled) ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect,
				InWidgetStyle.GetColorAndOpacityTint() * Style->HandleHighlightBrush.TintColor.GetSpecifiedColor()
			);
		}
	}

	return MaxLayerId;
}

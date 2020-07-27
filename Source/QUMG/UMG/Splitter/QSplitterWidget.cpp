// Fill out your copyright notice in the Description page of Project Settings.

#include "QSplitterWidget.h"

#include "Slate.h"
#include "Widgets/Layout/SConstraintCanvas.h"
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
const FText UQSplitterWidget::GetPaletteCategory()
{
	return NSLOCTEXT("UContenSlatetWidget", "QUI", "QUI");
}
#endif

TSharedRef<SWidget> UQSplitterWidget::RebuildWidget()
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

// Fill out your copyright notice in the Description page of Project Settings.

#include "QDialogContainer.h"

#include "Slate.h"


#include "Components/BorderSlot.h"

#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

#include "Components/GridPanel.h"
#include "Components/GridSlot.h"

#include "Components/Spacer.h"

#include "Components/TextBlock.h"

#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"

#define LOCTEXT_NAMESPACE "UMG"

UQDialogContainer::UQDialogContainer(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	TitleBrushColor = FLinearColor::Black;
	BorderColor = FLinearColor::Blue;

	BorderBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	BorderLineBrush.DrawAs = ESlateBrushDrawType::Box;
	BorderLineBrush.SetImageSize(FVector2D(2.0f, 2.0f));
	
	DefaultFont.Size = 10;

	this->OnClose().AddLambda([=](UQDialogContainer* dialog)
		{
			this->OnClose_BP.Broadcast(dialog);
		});
}

UQDialogContainer::UQDialogContainer() 
{
}

#if WITH_EDITOR
const FText UQDialogContainer::GetPaletteCategory()
{
	return LOCTEXT("zuomieye", "zuomieye");
}
#endif

TSharedRef<SWidget> UQDialogContainer::RebuildWidget()
{
	TSharedRef<SBorder, ESPMode::NotThreadSafe > temprorary = 
		SNew(SBorder)
			[
				SNew(SBorder)
				[
					SNew(SHorizontalBox) +
					SHorizontalBox::Slot()
					[
						SNew(STextBlock).Text(TitleText).Font(DefaultFont)
					]
					.FillWidth(1)
					.VAlign(EVerticalAlignment::VAlign_Center) +
					SHorizontalBox::Slot()
					[
						SNew(SButton)
						[
							SNew(STextBlock).Text(LOCTEXT("X", "X")).Font(DefaultFont)
						]
					.ContentPadding(FMargin{ 10,0 })
					.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, HandleCloseBtnClick))
					].AutoWidth()
					.HAlign(EHorizontalAlignment::HAlign_Right)
				]
				.BorderImage(&TitleBrush)
				.OnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleTitleBarMouseDown))
				.OnMouseButtonUp(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleTitleBarMouseUp))
				.ForegroundColor(FLinearColor(FColor{ 0xf0, 0xf0, 0xf0 }))
				.BorderBackgroundColor(TitleBrushColor)
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				.VAlign(EVerticalAlignment::VAlign_Center)
			].Padding(FMargin{ 0.0,0.0,0.0,1.0f }).BorderImage(&BorderLineBrush);


	//border start
	Border_TL = SNew(SBorder)
				[
					SNew(SBox)
					[
						SNew(SImage).Image(&BorderLineBrush)
					].WidthOverride(DialogPadding.Left)
					 .HeightOverride(DialogPadding.Top)
					 .VAlign(EVerticalAlignment::VAlign_Bottom)
					 .HAlign(EHorizontalAlignment::HAlign_Right)
				] .BorderImage(&BorderBrush)
				  .Cursor(EMouseCursor::ResizeSouthEast)
				  .OnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleBorderAreaDown))
		          .Padding(FMargin{ 0.0f });

	Border_TC = SNew(SBorder).BorderImage(&BorderBrush)
							 .Cursor(EMouseCursor::ResizeUpDown)
							 .OnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleBorderAreaDown))
							 [
							 	SNew(SImage).Image(&BorderLineBrush)
							 ].VAlign(EVerticalAlignment::VAlign_Bottom) 
							  .Padding(FMargin{ 0.0f });

	Border_TR = SNew(SBorder)
				[
					SNew(SBox)
					[
						SNew(SImage).Image(&BorderLineBrush)
					].WidthOverride(DialogPadding.Right)
				     .HeightOverride(DialogPadding.Top)
					 .VAlign(EVerticalAlignment::VAlign_Bottom)
					 .HAlign(EHorizontalAlignment::HAlign_Left)
				]
				 .BorderImage(&BorderBrush)
				 .Cursor(EMouseCursor::ResizeSouthWest )
				 .OnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleBorderAreaDown))
				 .Padding(FMargin{ 0.0f });
				
	Border_MShortL = SNew(SBorder).BorderImage(&BorderBrush)
								  .Cursor(EMouseCursor::ResizeLeftRight)
								  .OnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleBorderAreaDown))
								  .OnMouseButtonUp(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleBorderAreaUp))
									[
										SNew(SImage).Image(&BorderLineBrush)
									]
								  .HAlign(EHorizontalAlignment::HAlign_Right)
								  .Padding(FMargin{ 0.0f });


	Border_MShortR = SNew(SBorder).BorderImage(&BorderBrush)
								  .Cursor(EMouseCursor::ResizeLeftRight)
								  .OnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleBorderAreaDown))
								  .OnMouseButtonUp(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleBorderAreaUp))
								  [
									SNew(SImage).Image(&BorderLineBrush)
								  ]
								  .HAlign(EHorizontalAlignment::HAlign_Left)
								  .Padding(FMargin{ 0.0f });


	Border_ML = SNew(SBorder).BorderImage(&BorderBrush)
							 .Cursor(EMouseCursor::ResizeLeftRight)
							 .OnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleBorderAreaDown))
							 .OnMouseButtonUp(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleBorderAreaUp))
							 [
							 	SNew(SImage).Image(&BorderLineBrush)
							 ]
							 .HAlign(EHorizontalAlignment::HAlign_Right)
							 .Padding(FMargin{ 0.0f });

	Border_MR = SNew(SBorder).BorderImage(&BorderBrush)
							 .Cursor(EMouseCursor::ResizeLeftRight)
							 .OnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleBorderAreaDown))
							 .OnMouseButtonUp(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleBorderAreaUp))
							 	[
							 		SNew(SImage).Image(&BorderLineBrush)
							 	]
							 .HAlign(EHorizontalAlignment::HAlign_Left)
							 .Padding(FMargin{ 0.0f });

	Border_BL = SNew(SBorder)
	[
		SNew(SBox)
		[
			SNew(SImage).Image(&BorderLineBrush)
		].WidthOverride(DialogPadding.Left)
		.HeightOverride(DialogPadding.Bottom)
		.VAlign(EVerticalAlignment::VAlign_Top)
		.HAlign(EHorizontalAlignment::HAlign_Right)

	].BorderImage(&BorderBrush)
     .Cursor(EMouseCursor::ResizeSouthWest)
	 .OnMouseButtonDown(BIND_UOBJECT_DELEGATE (	FPointerEventHandler, HandleBorderAreaDown))
	 .OnMouseButtonUp(BIND_UOBJECT_DELEGATE(	FPointerEventHandler, HandleBorderAreaUp))
	 .Padding(FMargin{ 0.0f });
	
	Border_BC = SNew(SBorder).BorderImage(&BorderBrush)
		                     .Cursor(EMouseCursor::ResizeUpDown)
							 .OnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleBorderAreaDown))
							 .OnMouseButtonUp(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleBorderAreaUp))
							 [
							 	SNew(SImage).Image(&BorderLineBrush)
							 ].VAlign(EVerticalAlignment::VAlign_Top)
							  .Padding(FMargin{ 0.0f });

	Border_BR = SNew(SBorder)
	[
		SNew(SBox)
		[
			SNew(SImage).Image(&BorderLineBrush)
		].WidthOverride(DialogPadding.Right)
		 .HeightOverride(DialogPadding.Bottom)
		 .VAlign(EVerticalAlignment::VAlign_Top)
		 .HAlign(EHorizontalAlignment::HAlign_Left)

	].BorderImage(&BorderBrush)
	 .Cursor(EMouseCursor::ResizeSouthEast)
	 .OnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleBorderAreaDown))
	 .OnMouseButtonUp(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleBorderAreaUp))
	 .Padding(FMargin{ 0.0f });

	//border end

	MyBorder = SNew(SBorder);
														   //标题区域				
	TSharedRef<SGridPanel> Title_Grid = SNew(SGridPanel) + SGridPanel::Slot(0, 0)[Border_TL.ToSharedRef()]
														 + SGridPanel::Slot(1, 0)[Border_TC.ToSharedRef()]
														 + SGridPanel::Slot(2, 0)[Border_TR.ToSharedRef()]
														 + SGridPanel::Slot(0, 1)[Border_MShortL.ToSharedRef()]
														 + SGridPanel::Slot(1, 1)[temprorary]
														 + SGridPanel::Slot(2, 1)[Border_MShortR.ToSharedRef()]
														   //内容区域
														 + SGridPanel::Slot(0, 2)[Border_ML.ToSharedRef()].Padding(FMargin{ 0.0f })
														 + SGridPanel::Slot(1, 2).Padding(FMargin{ 0.0f })
														 [
														 	MyBorder.ToSharedRef()
														 ].Padding(FMargin{ 0.0f })
														 + SGridPanel::Slot(2, 2)[Border_MR.ToSharedRef()].Padding(FMargin{ 0.0f })
														 + SGridPanel::Slot(0, 3)[Border_BL.ToSharedRef()].Padding(FMargin{ 0.0f })
														 + SGridPanel::Slot(1, 3)[Border_BC.ToSharedRef()].Padding(FMargin{ 0.0f })
														 + SGridPanel::Slot(2, 3)[Border_BR.ToSharedRef()].Padding(FMargin{ 0.0f });

	Title_Grid->SetColumnFill(0, 0.0f);
	Title_Grid->SetColumnFill(1, 1.0f);
	Title_Grid->SetColumnFill(2, 0.0f);

	Title_Grid->SetRowFill(0, 0.0f);
	Title_Grid->SetRowFill(1, 0.0f);

	Title_Grid->SetRowFill(2, 1.0f);
	Title_Grid->SetRowFill(3, 0.0f);


	auto ret_wgt = SNew(SBorder)[ Title_Grid ]
								.BorderImage(&BorderBrush).Padding(FMargin{ 0.0f })
								.Padding(FMargin{ 0.0f });

	if (GetChildrenCount() > 0)
	{
		Cast<UBorderSlot>(GetContentSlot())->BuildSlot(MyBorder.ToSharedRef());
	}

	return ret_wgt;
}

FReply UQDialogContainer::HandleTitleBarMouseDown(const FGeometry &geo, const FPointerEvent &ev)
{
	OnTitleMouseDown().Broadcast(this, geo, ev);
	return FReply::Handled();
}

FReply UQDialogContainer::HandleTitleBarMouseUp(const FGeometry &geo, const FPointerEvent &ev)
{
	OnTitleMouseUp().Broadcast(this, geo, ev);

	return FReply::Handled();
}

FReply UQDialogContainer::HandleMouseMove(const FGeometry & geo, const FPointerEvent & ev)
{
	return FReply::Handled();
}

FReply UQDialogContainer::HandleBorderAreaDown(const FGeometry & geo, const FPointerEvent & ev)
{
	//方向判断

	if (Border_TL->IsHovered())
	{
		//西北 North Western
		OnResizingSize().Broadcast(this, EM_DialogScaleDirection::EM_Decrease, EM_DialogScaleDirection::EM_Decrease, geo, ev);
	}

	if (Border_TC->IsHovered())
	{
		//北 North
		OnResizingSize().Broadcast(this, EM_DialogScaleDirection::EM_None, EM_DialogScaleDirection::EM_Decrease, geo, ev);
	}

	if (Border_TR->IsHovered())
	{
		//东北 North Eastern
		OnResizingSize().Broadcast(this, EM_DialogScaleDirection::EM_Increase, EM_DialogScaleDirection::EM_Decrease, geo, ev);
	}

	if (Border_ML->IsHovered() || Border_MShortL->IsHovered() )
	{
		//西 West
		OnResizingSize().Broadcast(this, EM_DialogScaleDirection::EM_Decrease, EM_DialogScaleDirection::EM_None, geo, ev);
	}

	if (Border_MR->IsHovered() || Border_MShortR->IsHovered() )
	{
		//东  East
		OnResizingSize().Broadcast(this, EM_DialogScaleDirection::EM_Increase, EM_DialogScaleDirection::EM_None, geo, ev);
	}

	if (Border_BL->IsHovered())
	{
		//西南 South Western
		OnResizingSize().Broadcast(this, EM_DialogScaleDirection::EM_Decrease, EM_DialogScaleDirection::EM_Increase, geo, ev);
	}

	if (Border_BC->IsHovered())
	{
		//南 South
		OnResizingSize().Broadcast(this, EM_DialogScaleDirection::EM_None, EM_DialogScaleDirection::EM_Increase, geo, ev);
	}

	if (Border_BR->IsHovered())
	{
		//东南 South Eastern
		OnResizingSize().Broadcast(this, EM_DialogScaleDirection::EM_Increase, EM_DialogScaleDirection::EM_Increase, geo, ev);
	}

	return FReply::Handled();
}

FReply UQDialogContainer::HandleBorderAreaUp(const FGeometry & geo, const FPointerEvent & ev)
{
	OnResizeSizeComplated().Broadcast(this, geo, ev);
	return FReply::Handled();
}

FReply UQDialogContainer::HandleCloseBtnClick()
{
	OnClose().Broadcast(this);
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
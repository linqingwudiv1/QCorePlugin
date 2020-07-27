// Fill out your copyright notice in the Description page of Project Settings.


#include "QSpliter.h"

#include "QSplitterWidget.h"

#include "Components/Border.h"
#include "Runtime/Slate/Public/Widgets/Layout/SBorder.h"

#include "Runtime/UMG/Public/Components/PanelSlot.h"
#include "umg/Splitter/QSplitterSlot.h"

#include "Runtime/UMG/Public/Components/HorizontalBox.h"

#define LOCTEXT_NAMESPACE "UMG"

#if WITH_EDITOR
const FText UQSpliter::GetPaletteCategory()
{
	//UE_LOG(LogTemp, Log, TEXT(" GetPaletteCategory "));
	
	return LOCTEXT("QUI", "QUI");
}
#endif

void UQSpliter::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MySplitter.Reset();
}

int32 UQSpliter::GetChildrenCountByNotCollapsed() const
{
	if (!MySplitter)
	{
		return INDEX_NONE;
	}

	int32 count = 0;
	FChildren* Children = MySplitter->GetChildren();

	for (int32 i = 0;  i < this->GetChildrenCount();i ++ )
	{
		UWidget* wgt = GetChildAt(i);
		if ( ESlateVisibility::Collapsed != wgt->GetVisibility() )
		{
			count++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[cccc] : %f"), this->GetChildrenCount());

	return count;
}

float UQSpliter::GetCoefficientCountByNotCollapsed() const
{
	if ( !MySplitter )
	{
		return INDEX_NONE;
	}

	float count = 0;
	TArray<UPanelSlot*> arr_slots = this->GetSlots();

	for (int32 i = 0; i < arr_slots.Num() ; i++)
	{
		UWidget* wgt = GetChildAt(i);
		if ( ESlateVisibility::Collapsed != wgt->GetVisibility() )
		{
			UQSplitterSlot* temp_slot = Cast<UQSplitterSlot>(arr_slots[i]);

			if ( !temp_slot ) 
			{
				continue;
			}

			count += temp_slot->SizeValue;
		}
	}

	return count;
}

EOrientation UQSpliter::GetEOrientation() const
{
	switch (this->Direction)
	{
		case EM_SplitterDirectionType::EM_Vertical:
		{
			return EOrientation::Orient_Vertical;
		}
		case EM_SplitterDirectionType::EM_Horizontal:
		{
			return EOrientation::Orient_Horizontal;
		}
		default:
		{
			return EOrientation::Orient_Horizontal;
		}
	}
}

ESplitterResizeMode::Type UQSpliter::GetESplitterResizeMode() const
{
	switch (this->ResizeMode)
	{
		case EM_SplitterResizeMode::EM_FixedPosition:
		{
			return ESplitterResizeMode::FixedPosition;
		}
		case EM_SplitterResizeMode::EM_FixedSize:
		{
			return ESplitterResizeMode::FixedSize;
		}
		case EM_SplitterResizeMode::EM_Fill:
		{
			return ESplitterResizeMode::Fill;
		}
		default:
		{
			return ESplitterResizeMode::FixedPosition;
		}
	}
}

UClass * UQSpliter::GetSlotClass() const
{
	UE_LOG(LogTemp, Log, TEXT(" GetSlotClass "));

	return UQSplitterSlot::StaticClass();
}

void UQSpliter::OnSlotAdded(UPanelSlot * slot)
{
	if (!MySplitter.IsValid())
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT(" OnSlotAdded "));
	
	CastChecked< UQSplitterSlot>(slot)->BuildSlot(MySplitter.ToSharedRef());
}

void UQSpliter::OnSlotRemoved(UPanelSlot * slot)
{
	// develop 
	TSharedPtr<SWidget> Widget = slot->Content->GetCachedWidget();

	if ( !MySplitter.IsValid() || 
		 !Widget.IsValid() )
	{
		return;
	}

	FChildren* Children = MySplitter->GetChildren();

	for (int32 i = 0; i < Children->Num(); i++ ) 
	{
		TSharedRef<SWidget> tempWgt = Children->GetChildAt(i);

		if (Widget == tempWgt)
		{
			MySplitter->RemoveAt(i);
			break;
		}
	}
}

void UQSpliter::Handle_OnSplitteItemResize(UWidget* wgt,float val)
{
	OnSplitteItemResize.Broadcast(wgt, val);

	UE_LOG(LogTemp, Log, TEXT(" Handle_OnSplitteItemResize "));
}

TSharedRef<SWidget> UQSpliter::RebuildWidget()
{
	MySplitter = SNew(SSplitterEx).Orientation ( this->GetEOrientation() )
								  .PhysicalSplitterHandleSize ( HandSize )
								  .HitDetectionSplitterHandleSize ( HandSize )
								  .ResizeMode ( this->GetESplitterResizeMode() );

	for ( UPanelSlot* PanelSlot : Slots ) 
	{
		if (UQSplitterSlot* TypedSlot = Cast<UQSplitterSlot>(PanelSlot))
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot( MySplitter.ToSharedRef() );
			TypedSlot->OnSplitteItemResize().AddUObject(this, &UQSpliter::Handle_OnSplitteItemResize);
		}
	}

	return MySplitter.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE




int32 SSplitterEx::OnPaint(const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "CppWgt_SpliterComponent.h"

#include "CppWgt_BaseSplitter.h"

#include "Components/Border.h"
#include "Runtime/Slate/Public/Widgets/Layout/SBorder.h"

#include "Runtime/UMG/Public/Components/PanelSlot.h"
#include "umg/Splitter/SplitterComponentSlot.h"
#include "SplitterComponentSlot.h"

#include "Runtime/UMG/Public/Components/HorizontalBox.h"

#define LOCTEXT_NAMESPACE "UMG"

#if WITH_EDITOR
const FText UCppWgt_SpliterComponent::GetPaletteCategory()
{
	//UE_LOG(LogTemp, Log, TEXT(" GetPaletteCategory "));
	
	return LOCTEXT("", "QingUI");
}
#endif

void UCppWgt_SpliterComponent::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MySplitter.Reset();
}


int32 UCppWgt_SpliterComponent::GetChildrenCountByNotCollapsed() const
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

float UCppWgt_SpliterComponent::GetCoefficientCountByNotCollapsed() const
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
			USplitterComponentSlot* temp_slot = Cast<USplitterComponentSlot>(arr_slots[i]);

			if ( !temp_slot ) 
			{
				continue;
			}

			count += temp_slot->SizeValue;
		}
	}

	return count;
}

EOrientation UCppWgt_SpliterComponent::GetEOrientation() const
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

ESplitterResizeMode::Type UCppWgt_SpliterComponent::GetESplitterResizeMode() const
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

UClass * UCppWgt_SpliterComponent::GetSlotClass() const
{
	UE_LOG(LogTemp, Log, TEXT(" GetSlotClass "));

	return USplitterComponentSlot::StaticClass();
}

void UCppWgt_SpliterComponent::OnSlotAdded(UPanelSlot * Slot)
{

	if (!MySplitter.IsValid())
	{
		return;
	}


	UE_LOG(LogTemp, Log, TEXT(" OnSlotAdded "));

	CastChecked< USplitterComponentSlot>(Slot)->BuildSlot(MySplitter.ToSharedRef());
}

void UCppWgt_SpliterComponent::OnSlotRemoved(UPanelSlot * Slot)
{
	// develop 
	TSharedPtr<SWidget> Widget = Slot->Content->GetCachedWidget();

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

void UCppWgt_SpliterComponent::Handle_OnSplitteItemResize(UWidget* wgt,float val)
{
	OnSplitteItemResize.Broadcast(wgt, val);

	UE_LOG(LogTemp, Log, TEXT(" Handle_OnSplitteItemResize "));
}

TSharedRef<SWidget> UCppWgt_SpliterComponent::RebuildWidget()
{
	MySplitter = SNew(SSplitterEx).Orientation ( this->GetEOrientation() )
								  .PhysicalSplitterHandleSize ( HandSize )
								  .HitDetectionSplitterHandleSize ( HandSize )
								  .ResizeMode ( this->GetESplitterResizeMode() );

	for ( UPanelSlot* PanelSlot : Slots ) 
	{
		if (USplitterComponentSlot* TypedSlot = Cast<USplitterComponentSlot>(PanelSlot))
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot( MySplitter.ToSharedRef() );
			TypedSlot->OnSplitteItemResize().AddUObject(this, &UCppWgt_SpliterComponent::Handle_OnSplitteItemResize);
		}
	}

	return MySplitter.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE
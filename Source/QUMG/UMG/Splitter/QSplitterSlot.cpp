// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "QSplitterSlot.h"


#include "QSpliter.h"

#include "Components/Widget.h"

#include "Engine/Engine.h"
#include "Components/PanelWidget.h"



/////////////////////////////////////////////////////
// UHorizontalBoxSlot

UQSplitterSlot::UQSplitterSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Slot = NULL;
}

void UQSplitterSlot::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	Slot = NULL;
}

void UQSplitterSlot::Handle_OnSlotResized(float val)
{
	if (Slot == nullptr)
	{
		return;
	}

	Slot->Value( val );
	SizeValue = val;

	this->OnSplitteItemResize().Broadcast(Content, val);
}

UWidget * UQSplitterSlot::GetContentWidget() const
{
	return this->Content;
}

float UQSplitterSlot::GetSize() const
{
	UQSpliter* splitter = Cast<UQSpliter>(this->Parent);

	FVector2D size = splitter->GetCachedGeometry ().GetLocalSize();

	float len = 0.0f;
	switch ( splitter->Direction )
	{
		case EM_SplitterDirectionType::EM_Horizontal: 
		{
			len = size.X;
			break;
		}
		case EM_SplitterDirectionType::EM_Vertical: 
		{
			len = size.Y;
			break;
		}
		default:
		{
			len = size.Y;
			break;
		}
	}

	float factor = 0.0f;

	for (auto slot : splitter->GetSlots())
	{
		UQSplitterSlot* SplitterSlot = Cast<UQSplitterSlot>(slot);

		if ( SplitterSlot == nullptr )
		{
			continue;
		}

		factor += SplitterSlot->SizeValue;
	}

	float ret_len = ( len / factor ) * SizeValue;

	return ret_len;
}

void UQSplitterSlot::BuildSlot(TSharedRef<SSplitter> SplitterCom)
{
	Slot = &SplitterCom->AddSlot()
	[
		Content == NULL ? SNullWidget::NullWidget : Content->TakeWidget()
	].Value(SizeValue);

	TBaseDelegate<void, float> delegate_event;

	delegate_event.BindUObject(this, &UQSplitterSlot::Handle_OnSlotResized);

	Slot->OnSlotResized( delegate_event );
}

void UQSplitterSlot::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

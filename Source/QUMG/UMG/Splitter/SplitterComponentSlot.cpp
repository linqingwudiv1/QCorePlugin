// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SplitterComponentSlot.h"


#include "CppWgt_SpliterComponent.h"

#include "Components/Widget.h"

#include "Engine/Engine.h"
#include "Components/PanelWidget.h"



/////////////////////////////////////////////////////
// UHorizontalBoxSlot

USplitterComponentSlot::USplitterComponentSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Slot = NULL;
}

void USplitterComponentSlot::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	Slot = NULL;
}

void USplitterComponentSlot::Handle_OnSlotResized(float val)
{
	if (Slot == nullptr)
	{
		return;
	}

	Slot->Value( val );
	SizeValue = val;

	this->OnSplitteItemResize().Broadcast(Content, val);
}

UWidget * USplitterComponentSlot::GetContentWidget() const
{
	return this->Content;
}

float USplitterComponentSlot::GetSize() const
{
	UCppWgt_SpliterComponent* splitter = Cast<UCppWgt_SpliterComponent>(this->Parent);

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
		USplitterComponentSlot* SplitterSlot = Cast<USplitterComponentSlot>(slot);

		if ( SplitterSlot == nullptr )
		{
			continue;
		}

		factor += SplitterSlot->SizeValue;
	}

	float ret_len = ( len / factor ) * SizeValue;

	return ret_len;
}

void USplitterComponentSlot::BuildSlot(TSharedRef<SSplitter> SplitterCom)
{
	Slot = &SplitterCom->AddSlot()
	[
		Content == NULL ? SNullWidget::NullWidget : Content->TakeWidget()
	].Value(SizeValue);

	TBaseDelegate<void, float> delegate_event;

	delegate_event.BindUObject(this, &USplitterComponentSlot::Handle_OnSlotResized);

	Slot->OnSlotResized( delegate_event );
}

void USplitterComponentSlot::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

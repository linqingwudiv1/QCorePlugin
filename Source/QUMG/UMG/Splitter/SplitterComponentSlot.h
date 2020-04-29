// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "Components/PanelSlot.h"
#include "Components/SlateWrapperTypes.h"

#include "Runtime/Slate/Public/Widgets/Layout/SSplitter.h"

#include "SplitterComponentSlot.generated.h"



UCLASS()
class QUMG_API USplitterComponentSlot : public UPanelSlot
{
	GENERATED_UCLASS_BODY()
public :
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layout|Spliter Slot")
		float SizeValue = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Variant")
		///	Unit DPI
		float MinSize = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Variant")
		/// Unit DPI
		float MaxSize = 0.0f;
public:
	// Event
	DECLARE_EVENT_TwoParams(FString, FEvent_OnSplitteItemResize,UWidget* , float )
	FEvent_OnSplitteItemResize &  OnSplitteItemResize() { return Event_OnSplitteItemResize; }

public:
	FEvent_OnSplitteItemResize Event_OnSplitteItemResize;
public:

	UFUNCTION(BlueprintCallable, Category = "Slot | Spliter Slot")
		UWidget * GetContentWidget() const;

	UFUNCTION(BlueprintCallable, Category = "Slot | Spliter Slot")
		float GetSize() const;
public:
	void BuildSlot(TSharedRef<SSplitter> SplitterCom);

	// UPanelSlot interface
	virtual void SynchronizeProperties() override;
	// End of UPanelSlot interface

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

protected:
	void Handle_OnSlotResized(float val);

private:
	SSplitter::FSlot* Slot;
};

// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "Components/PanelSlot.h"
#include "Components/SlateWrapperTypes.h"

#include "Widgets/Layout/SSplitter.h"

#include "QSplitterSlot.generated.h"

UCLASS()
class QUMG_API UQSplitterSlot : public UPanelSlot
{
	GENERATED_UCLASS_BODY()
public :
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot | QSpliter Slot")
		float SizeValue = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot | QSpliter Slot")
		///	Unit DPI
		float MinSize = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot | QSpliter Slot")
		/// Unit DPI
		float MaxSize = 0.0f;

public:
#pragma region event
	DECLARE_EVENT_TwoParams(FString, FEvent_OnSplitteItemResize, UWidget*, float)
	FEvent_OnSplitteItemResize& OnSplitteItemResize() { return Event_OnSplitteItemResize; }

protected:
	FEvent_OnSplitteItemResize Event_OnSplitteItemResize;
#pragma endregion event

public:

	UFUNCTION(BlueprintCallable, Category = "Slot | QSpliter Slot")
		// 
		UWidget * GetContentWidget() const;

	UFUNCTION(BlueprintCallable, Category = "Slot | QSpliter Slot")
		// 
		float GetSize() const;
public:
	// 
	void BuildSlot(TSharedRef<SSplitter> SplitterCom);

	#pragma region UPanelSlot Interface

	// UPanelSlot interface
	virtual void SynchronizeProperties() override;
	// End of UPanelSlot interface

	#pragma endregion UPanelSlot Interface


	// 
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

protected:
	void Handle_OnSlotResized(float val);

private:
	SSplitter::FSlot* Slot;
};

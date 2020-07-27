// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Interface/IQDialogInterface.h"

#include "QDialogTitlebar.generated.h"

/**
 * 
 */
 class UBorder;
UCLASS(Abstract)
class QUMG_API UQDialogTitlebar : public UUserWidget , public IQDialogInterface
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite , EditAnywhere , Category = "QDialog")
		FText Title ;

protected:

#pragma region interface
	UFUNCTION()
	virtual void Close();

#pragma endregion interface


#pragma region UserWidget virtual function

	/**  */
	virtual void NativePreConstruct() override;

#pragma endregion UserWidget virtual function

	#pragma region Event

public:
	DECLARE_EVENT(FString, FEvent_OnClose);
	FEvent_OnClose& OnClose()
	{
		return Event_OnClose;
	}
protected:
	FEvent_OnClose Event_OnClose;

	public:
	DECLARE_EVENT_TwoParams(FString ,FEvent_OnTileMouseButtonUp, const FGeometry& , const FPointerEvent& );
		FEvent_OnTileMouseButtonUp & OnTitleMouseButtonUp()
		{
			return Event_OnTileMouseUp;
		}
	protected:
		FEvent_OnTileMouseButtonUp Event_OnTileMouseUp;

public:
	DECLARE_EVENT_TwoParams(FString ,FEvent_OnTileMouseButtonDown, const FGeometry& , const FPointerEvent& );
	FEvent_OnTileMouseButtonDown& OnTitleMouseButtonDown()
	{
		return Event_OnTileMouseDown;
	}
	//DECLARE_EVENT_TwoParams(FString, FEvent_OnTileMouseButtonDown, const FGeometry&, const FPointerEvent&);
	
protected:
	FEvent_OnTileMouseButtonDown Event_OnTileMouseDown;


	#pragma endregion Event
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CppWgt_BaseUserWidget.generated.h"



//DECLARE_MULTICAST_DELEGATE(FOnDialogClose);

/**
 * 
 */
UCLASS(Abstract, editinlinenew, BlueprintType, Blueprintable, meta = (DontUseGenericSpawnObject = "True", DisableNativeTick))
class QCOREPLUGIN_API UCppWgt_BaseUserWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Event

public:
	DECLARE_EVENT(FString, FOnInitCompleted);
	FOnInitCompleted& OnInitCompleted() {return this->Event_InitCompleted;}
protected:
	FOnInitCompleted Event_InitCompleted;

#pragma endregion

protected:
	/***/
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	/***/
	virtual void NativeConstruct();
};

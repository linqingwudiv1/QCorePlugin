// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "data/QUI_TabItem.h"
#include "CppWgt_BaseTabContainerBtn.generated.h"

/**
 *
 */
UCLASS(Abstract)
class QUMG_API UCppWgt_BaseTabContainerBtn : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "TabBtnCommponent")
		void HandleOnClick();

	DECLARE_EVENT_TwoParams(FString, FEvent_OnTabBtnClick,const UCppWgt_BaseTabContainerBtn*, int32)
		FEvent_OnTabBtnClick& OnTabBtnClick() { return Event_OnTabBtnClick; }

		static UCppWgt_BaseTabContainerBtn* CreateUMG(UObject *WorldObjectContext, TSubclassOf<UCppWgt_BaseTabContainerBtn> cla,const FQUI_TabItem& data);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CppVariant")
		FQUI_TabItem Data;
protected:
	FEvent_OnTabBtnClick Event_OnTabBtnClick;
};

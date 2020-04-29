// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "data/QUI_TabItem.h"
#include "CppWgt_BaseTabContainer.generated.h"

/**
 *
 */
UCLASS(Abstract)
class  UCppWgt_BaseTabContainer : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual int32 NativePaint(const FPaintArgs& Args, 
							  const FGeometry& AllottedGeometry, 
							  const FSlateRect& MyCullingRect, 
							  FSlateWindowElementList& OutDrawElements, 
							  int32 LayerId, 
							  const FWidgetStyle& InWidgetStyle, 
							  bool bParentEnabled) const override;
public:

	UFUNCTION(BlueprintCallable, Category = "Tab Container")
		class UPanelSlot* Add(FQUI_TabItem tabdata,class UUserWidget* wgt);

	UFUNCTION(BlueprintCallable, Category = "Tab Container")
		void Remove(int32 index);

	UFUNCTION(BlueprintCallable, Category = "Tab Container")
		void Switch(int32 index);

	/**  **/
	UFUNCTION(BlueprintCallable, Category = "Tab Container")
		bool Clear();

	/**  **/
	UFUNCTION(BlueprintCallable, Category = "Tab Container")
		bool RemoveAll();
public:
	//event 
	
	DECLARE_EVENT_TwoParams(FString, FEvent_OnTabChange, const UCppWgt_BaseTabContainer*, int32)
	FEvent_OnTabChange& OnTabChange() { return Event_OnTabChange; }
protected:
		virtual void HandleItemTabBtnClick(const class UCppWgt_BaseTabContainerBtn* target, int32 index);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CppVariant")
		int32 Current_Index = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CppVariant")
		class UWidgetSwitcher* ContentContainer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CppVariant")
		class UPanelWidget* TabContainer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CppVariant")
		TSubclassOf<class UCppWgt_BaseTabContainerBtn> TabBtnClass;

protected:
	FEvent_OnTabChange Event_OnTabChange;
};

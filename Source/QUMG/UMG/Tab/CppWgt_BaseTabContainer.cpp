// Fill out your copyright notice in the Description page of Project Settings.
#include "CppWgt_BaseTabContainer.h"

#include "Components/PanelWidget.h"
#include "CppWgt_BaseTabContainerBtn.h"
#include "Components/WidgetSwitcher.h"

void UCppWgt_BaseTabContainer::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	UE_LOG(LogTemp, Log, TEXT("NativeOnInitialized"));
}

void UCppWgt_BaseTabContainer::NativePreConstruct()
{
	Super::NativePreConstruct();
	UE_LOG(LogTemp, Log, TEXT("NativePreConstruct"));
}

void UCppWgt_BaseTabContainer::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("NativeConstruct"));
}

void UCppWgt_BaseTabContainer::NativeDestruct()
{
	Super::NativeDestruct();
	UE_LOG(LogTemp, Log, TEXT("NativeDestruct"));
}

void UCppWgt_BaseTabContainer::NativeTick(const FGeometry & MyGeometry, float InDeltaTime)
{
	static bool wait_once = true;
	if (wait_once)
	{
		UE_LOG(LogTemp, Log, TEXT("NativeTick"));
		wait_once = false;
	}
	Super::NativeTick(MyGeometry, InDeltaTime);
}

int32 UCppWgt_BaseTabContainer::NativePaint(const FPaintArgs & Args	, 
											const FGeometry  & AllottedGeometry	, 
											const FSlateRect & MyCullingRect	, 
											FSlateWindowElementList & OutDrawElements	, 
											int32 LayerId	, 
											const FWidgetStyle & InWidgetStyle	, 
											bool bParentEnabled) const
{
	static bool wait_once = true;
	if (wait_once)
	{
		UE_LOG(LogTemp, Log, TEXT("NativePaint"));
		wait_once = false;
	}
	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

UPanelSlot*  UCppWgt_BaseTabContainer::Add(FQUI_TabItem tabdata, class UUserWidget* wgt)
{
	check(TabBtnClass);
	UCppWgt_BaseTabContainerBtn* btnitem_umg = UCppWgt_BaseTabContainerBtn::CreateUMG(this, TabBtnClass, tabdata);
	UPanelSlot *ret_slot = nullptr;

	if ( btnitem_umg != nullptr )
	{
		auto delegate1 = btnitem_umg->OnTabBtnClick().AddUObject(this, &UCppWgt_BaseTabContainer::HandleItemTabBtnClick);
		bool boolean = delegate1.IsValid();
		UE_LOG(LogTemp, Log, TEXT("is IsValid :%d"), boolean);
		ret_slot = TabContainer->AddChild(btnitem_umg);
	}

	ContentContainer->AddChild(wgt);

	return ret_slot;
}

void UCppWgt_BaseTabContainer::Remove(int32 index)
{
}

void UCppWgt_BaseTabContainer::Switch(int32 index)
{
	UCppWgt_BaseTabContainerBtn* oldBtn = (Current_Index == INDEX_NONE ? nullptr: Cast<UCppWgt_BaseTabContainerBtn>(TabContainer->GetChildAt(Current_Index)) );
	UCppWgt_BaseTabContainerBtn* btn = Cast<UCppWgt_BaseTabContainerBtn>(TabContainer->GetChildAt(index));

	if (oldBtn != nullptr) 
	{
		oldBtn->SetIsEnabled(true);
	}

	if (btn != nullptr) 
	{
		btn->SetIsEnabled(false);
	}

	Current_Index = index;

	ContentContainer->SetActiveWidgetIndex(index);
	
	OnTabChange().Broadcast(this, index);
}

bool UCppWgt_BaseTabContainer::Clear()
{
	bool ret_result = true;
	
	TabContainer->ClearChildren();
	ContentContainer->ClearChildren();
	Current_Index = INDEX_NONE;

	return ret_result;
}

bool UCppWgt_BaseTabContainer::RemoveAll()
{
	return this->Clear();
}

void UCppWgt_BaseTabContainer::HandleItemTabBtnClick(const UCppWgt_BaseTabContainerBtn* target, int32 index)
{
	UE_LOG(LogTemp, Log, TEXT(" tab click event  "));
	Switch(index);
}
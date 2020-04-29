// Fill out your copyright notice in the Description page of Project Settings.

#include "CppWgt_BaseTabContainerBtn.h"

#include "Components/PanelWidget.h"

void UCppWgt_BaseTabContainerBtn::HandleOnClick() 
{
	UPanelWidget* wgt = Cast<UPanelWidget>(this->GetParent());
	
	if (wgt == nullptr) 
	{
		UE_LOG(LogTemp, Error, TEXT("parent widget is nullptr.") );
	}

	int32 index = wgt->GetChildIndex(this);
	this->OnTabBtnClick().Broadcast(this, index);
}

UCppWgt_BaseTabContainerBtn* UCppWgt_BaseTabContainerBtn::CreateUMG(UObject *WorldObjectContext, TSubclassOf<UCppWgt_BaseTabContainerBtn> cla, const FQUI_TabItem& data)
{
	auto btnitem_umg = CreateWidget< UCppWgt_BaseTabContainerBtn>(WorldObjectContext->GetWorld(), cla);
	if (btnitem_umg == nullptr) 
	{
		return nullptr;
	}
	
	btnitem_umg->Data = data;
	return btnitem_umg;
}

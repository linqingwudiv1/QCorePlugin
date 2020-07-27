// Fill out your copyright notice in the Description page of Project Settings.


#include "CppWgt_BaseUserWidget.h"

FReply UCppWgt_BaseUserWidget::NativeOnMouseWheel(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent)
{
	return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}

void UCppWgt_BaseUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	OnInitCompleted().Broadcast();
}
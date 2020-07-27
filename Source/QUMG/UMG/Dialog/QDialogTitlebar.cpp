// Fill out your copyright notice in the Description page of Project Settings.


#include "QDialogTitlebar.h"

#include "Components/Border.h"

UQDialogTitlebar::UQDialogTitlebar(const FObjectInitializer & PCIP) 
: Super(PCIP)
{
}

void UQDialogTitlebar::Close()
{
	OnClose().Broadcast();
}

void UQDialogTitlebar::NativePreConstruct()
{
}

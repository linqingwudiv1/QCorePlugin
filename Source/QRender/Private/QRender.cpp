// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "QRender.h"
#define LOCTEXT_NAMESPACE "FQRenderModule"

void FRenderGModule::StartupModule()
{
	//UE_LOG(LogTemp, Log, TEXT("-------------------------------------------------------------------- QRender"))
}


void FRenderGModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRenderGModule, QRender);

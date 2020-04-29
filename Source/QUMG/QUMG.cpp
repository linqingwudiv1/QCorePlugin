// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "QUMG.h"
#define LOCTEXT_NAMESPACE "FQUMGModule"

void FQUMGModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("-------------------------------------------------------------------- QUMG"))
}


void FQUMGModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FQUMGModule, QUMG);

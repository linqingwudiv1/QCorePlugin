// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "QDesktopPlatformModule.h"
#include "DesktopPlatformPrivate.h"

IMPLEMENT_MODULE(FQDesktopPlatformModule, QDesktopPlatform );
DEFINE_LOG_CATEGORY(LogQDesktopPlatform);

void FQDesktopPlatformModule::StartupModule()
{
	DesktopPlatform = new FDesktopPlatform();
}

void FQDesktopPlatformModule::ShutdownModule()
{
	if (DesktopPlatform != NULL)
	{
		delete DesktopPlatform;
		DesktopPlatform = NULL;
	}
}

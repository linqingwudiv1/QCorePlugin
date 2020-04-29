// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "IDesktopPlatform.h"

class FQDesktopPlatformModule : public IModuleInterface
{
public:
	virtual void StartupModule();
	virtual void ShutdownModule();

	static IDesktopPlatform* Get()
	{
		FQDesktopPlatformModule& DesktopPlatformModule = FModuleManager::Get().LoadModuleChecked<FQDesktopPlatformModule>("QDesktopPlatform");
		return DesktopPlatformModule.GetSingleton();
	}

private:
	virtual IDesktopPlatform* GetSingleton() const { return DesktopPlatform; }

	IDesktopPlatform* DesktopPlatform;
};

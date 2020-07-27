// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class QDesktopPlatform : ModuleRules
{
    protected string PrivateDirectory
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "Private/")); }
    }

    public QDesktopPlatform(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = Path.GetFullPath(Path.Combine(PrivateDirectory, "DesktopPlatformPrivate.h"));

        PrivateIncludePaths.Add( PrivateDirectory );

		PublicIncludePaths.Add(Path.GetFullPath(Path.Combine(ModuleDirectory, "Public/")));

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"ApplicationCore",
				"Json",
			}
		);

		if (Target.IsInPlatformGroup(UnrealPlatformGroup.Linux))
		{
			PrivateIncludePathModuleNames.AddRange(
				new string[] {
					"QSlateFileDialogs",
				}
			);

			DynamicallyLoadedModuleNames.AddRange(
				new string[] {
					"QSlateFileDialogs",
				}
			);

			AddEngineThirdPartyPrivateStaticDependencies(Target, "SDL2");
		}
	}
}

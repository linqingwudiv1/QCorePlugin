// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
using System;
using System.IO;
using UnrealBuildTool;

public class QUMG : ModuleRules
{
    protected string PrivateDirectory
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "Private/")); }
    }


    public QUMG(ReadOnlyTargetRules Target) : base(Target)
    {
        //PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        //PrivatePCHHeaderFile = Path.GetFullPath(Path.Combine(ModuleDirectory, "QUMGPrivate.h"));

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "RHI",
                "RenderCore",
                "Slate",
                "SlateCore",
                "UMG"
            }
        );
    
        PrivateIncludePathModuleNames.Add("TargetPlatform");
    }
}

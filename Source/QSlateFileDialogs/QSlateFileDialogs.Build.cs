// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
using System;
using System.IO;
using UnrealBuildTool;

public class QSlateFileDialogs : ModuleRules
{
    protected string PrivateDirectory
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "Private/")); }
    }


    public QSlateFileDialogs(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = Path.GetFullPath(Path.Combine(PrivateDirectory, "SlateFileDialogsPrivate.h"));
        
        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core"              ,
                "CoreUObject"       ,
                "InputCore"         ,
                "Slate"             ,
                "SlateCore"         ,
                "DirectoryWatcher"  ,
            }
        );
    
        PrivateIncludePaths.AddRange(
            new string[] {
                PrivateDirectory,  
            }
        );

        PrivateIncludePathModuleNames.Add("TargetPlatform");
    }
}

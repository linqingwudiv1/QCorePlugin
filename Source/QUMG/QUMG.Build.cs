// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
using System;
using System.IO;
using UnrealBuildTool;

public class QUMG : ModuleRules
{
    /// <summary>
    /// 
    /// </summary>
    protected string PrivateDirectory
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "Private/")); }
    }

    public QUMG(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        //PrivatePCHHeaderFile = Path.GetFullPath(Path.Combine(ModuleDirectory, "QUMGPrivate.h"));

        PrivateIncludePaths.Add(PrivateDirectory);

        PublicIncludePaths.Add(Path.GetFullPath(ModuleDirectory));


        bEnforceIWYU = false;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core"          ,
                "CoreUObject"   ,
                "Engine"        ,
                "InputCore"     ,
                "RenderCore"    ,
                "RHI"           ,
                "Slate"         ,
                "SlateCore"     ,
                "UMG"           ,
                "WebBrowser"    ,
                "Http"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {

            });

        if (Target.bBuildEditor || Target.Platform == UnrealTargetPlatform.Android)
        {
            PrivateIncludePathModuleNames.AddRange(
                new string[]
                {
                    "WebBrowserTexture"
                }
            );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "WebBrowserTexture"
                }
            );
        }
    }
}

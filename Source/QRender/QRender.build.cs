// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
using System;
using System.IO;
using UnrealBuildTool;

public class QRender : ModuleRules
{
    /// <summary>
    /// 
    /// </summary>
    protected string PrivateDirectory
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "Private/")); }
    }

    public QRender(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateIncludePaths.AddRange(
            new string[] {
            }
        );

        Console.WriteLine(this.EngineDirectory);
        var AudioMixerInclude = Path.Combine(this.EngineDirectory, "Source/Runtime/AudioMixer/Private");
        Console.WriteLine("==================", AudioMixerInclude);
        PrivateIncludePaths.Add(AudioMixerInclude);

        PublicDependencyModuleNames.AddRange(new string[]
        {
                "OpenCVHelper",
                "OpenCV",
                "AudioMixer",
                "AVEncoder",
            //"GameplayMediaEncoder"
        });

        PublicDelayLoadDLLs.Add("mfplat.dll");
        PublicDelayLoadDLLs.Add("mfuuid.dll");
        PublicDelayLoadDLLs.Add("Mfreadwrite.dll");

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "ImageWrapper",
                    "InputCore",
                    "RenderCore",
                    "Renderer",
                    "Json",
                    "JsonUtilities",
					//"ImageWriteQueue",
					"OpenColorIO",
                    "OpenCVLensDistortion",
                    "OpenCVLensCalibration",
                    "RHI",
                    "Slate",
                    "SlateCore",
                    "Landscape",
                //"MessageLog",
                //"MeshUtilities2",
                //"JsEnv",
                //"PuertsExtra"
            }
        );

        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.Add("EditorFramework");
            PrivateDependencyModuleNames.Add("UnrealEd");
            PrivateDependencyModuleNames.Add("OpenColorIOEditor");
        }
    }
}

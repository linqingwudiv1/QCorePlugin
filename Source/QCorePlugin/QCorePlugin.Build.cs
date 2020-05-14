// Some copyright should be here...

using System;
using System.IO;
using UnrealBuildTool;

public class QCorePlugin : ModuleRules
{

    static private bool IsShipping(ReadOnlyTargetRules Target)
    {
        return Target.Configuration == UnrealTargetConfiguration.Shipping;
    }

    public QCorePlugin(ReadOnlyTargetRules Target) : base(Target)
	{


		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                // ... add other public dependencies that you statically link with here ...
                "QDesktopPlatform"
                
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Core"              ,
                "CoreUObject"       ,
                "Engine"            ,
                "InputCore"         ,
                "RHI"               ,
                "RenderCore"        ,
                "Slate"             ,
                "SlateCore"         ,
                "HTTP"              ,
                "UMG"               ,
                "AIModule"          ,
                "ImageWrapper"      ,
                "Json"              ,
                "JsonUtilities"     ,
                "Sockets"           ,
                "Networking"        ,
                "IPC"               ,
                // web plugin
                "WebBrowser"        ,
                "WebBrowserWidget"  ,
                // custom modules

				// ... add private dependencies that you statically link with here ...	
			}
			);

        if (IsShipping(Target))
        {
            //PrivateDependencyModuleNames.Add("QDesktopPlatform");
        }
        else
        {
            //PrivateDependencyModuleNames.Add("DesktopPlatform");
        }

        //WebBrowser Dependency
        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "UnrealEd"
                }
            );
        }

        PublicDefinitions.Add("WIN32_LEAN_AND_MEAN");

        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}

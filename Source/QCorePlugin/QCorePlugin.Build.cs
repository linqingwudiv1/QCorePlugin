// Some copyright should be here...

using UnrealBuildTool;

public class QCorePlugin : ModuleRules
{
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
				"Core",
                "WebBrowser",
                "WebBrowserWidget"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "Engine",
                "InputCore",
                "RHI",
                "RenderCore",
                "HTTP",
                "DesktopPlatform",
                "UMG",
                "Slate",
                "SlateCore",
                "AIModule",
                "ImageWrapper",
                "FunctionalTesting",
                "Json"              ,
                "JsonUtilities"     ,
                "Sockets",
                "Networking",
                "IPC"
				// ... add private dependencies that you statically link with here ...	
			}
			);

        PublicDefinitions.Add("WIN32_LEAN_AND_MEAN");

        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}

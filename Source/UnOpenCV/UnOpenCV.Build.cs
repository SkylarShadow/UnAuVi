// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class UnOpenCV : ModuleRules
{
	public UnOpenCV(ReadOnlyTargetRules Target) : base(Target)
	{

#region Load OpenCV
        string OpenCVIncPath = Path.Combine(ModuleDirectory, "../ThirdParty/OpenCV/include");
        string OpenCVLibPath = Path.Combine(ModuleDirectory, "../ThirdParty/OpenCV/lib");
        string OpenCVLibName = "opencv_world4120";

        PublicSystemIncludePaths.Add(OpenCVIncPath);
        PublicAdditionalLibraries.Add(Path.Combine(OpenCVLibPath, OpenCVLibName + ".lib")); //��̬��
        string OpenCVDLLName = OpenCVLibName + ".dll"; //��̬��
        PublicDelayLoadDLLs.Add(OpenCVDLLName);

        //string PlatformDir = Target.Platform.ToString(); //Windows64ϵͳΪ"Win64",���Ҫ�����ƽ̨���黹�ǲ������ַ�ʽ
        string BinaryPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../ThirdParty/OpenCV/bin"));
        RuntimeDependencies.Add(Path.Combine(BinaryPath, OpenCVDLLName));

        PublicDefinitions.Add("WITH_OPENCV=1");
#endregion
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
                "AssetRegistry",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Projects",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}

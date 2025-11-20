// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class UnFFmpeg : ModuleRules
{
	public UnFFmpeg(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

#region Load FFmpeg
        string FFmpegIncPath = Path.Combine(ModuleDirectory, "../../ThirdParty/FFmpeg/include");
        string FFmpegLibPath = Path.Combine(ModuleDirectory, "../../ThirdParty/FFmpeg/lib/x64");
        string FFmpegBinaryPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/FFmpeg/bin/x64"));
        PublicIncludePaths.Add(FFmpegIncPath);
        PublicDefinitions.Add("WITH_FFMPEG=1");

        PublicAdditionalLibraries.Add(Path.Combine(FFmpegLibPath, "avcodec.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(FFmpegLibPath, "avdevice.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(FFmpegLibPath, "avfilter.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(FFmpegLibPath, "avformat.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(FFmpegLibPath, "avutil.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(FFmpegLibPath, "swresample.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(FFmpegLibPath, "swscale.lib"));

        PublicDelayLoadDLLs.Add("avcodec.dll");
        PublicDelayLoadDLLs.Add("avdevice.dll");
        PublicDelayLoadDLLs.Add("avfilter.dll");
        PublicDelayLoadDLLs.Add("avformat.dll");
        PublicDelayLoadDLLs.Add("avutil.dll");
        PublicDelayLoadDLLs.Add("swresample.dll");
        PublicDelayLoadDLLs.Add("swscale.dll");
        PublicDelayLoadDLLs.Add("postproc.dll");

		//打包时有效
        RuntimeDependencies.Add("$(TargetOutputDir)/avcodec.dll", Path.Combine(FFmpegBinaryPath, "avcodec.dll"));
        RuntimeDependencies.Add("$(TargetOutputDir)/avdevice.dll", Path.Combine(FFmpegBinaryPath, "avdevice.dll"));
        RuntimeDependencies.Add("$(TargetOutputDir)/avfilter.dll", Path.Combine(FFmpegBinaryPath, "avfilter.dll"));
        RuntimeDependencies.Add("$(TargetOutputDir)/avformat.dll", Path.Combine(FFmpegBinaryPath, "avformat.dll"));
        RuntimeDependencies.Add("$(TargetOutputDir)/avutil.dll", Path.Combine(FFmpegBinaryPath, "avutil.dll"));
        RuntimeDependencies.Add("$(TargetOutputDir)/swresample.dll", Path.Combine(FFmpegBinaryPath, "swresample.dll"));
        RuntimeDependencies.Add("$(TargetOutputDir)/swscale.dll", Path.Combine(FFmpegBinaryPath, "swscale.dll"));
        RuntimeDependencies.Add("$(TargetOutputDir)/postproc.dll", Path.Combine(FFmpegBinaryPath, "postproc.dll"));

        RuntimeDependencies.Add("$(ProjectDir)/Binaries/Win64/avcodec.dll", Path.Combine(FFmpegBinaryPath, "avcodec.dll"));
        RuntimeDependencies.Add("$(ProjectDir)/Binaries/Win64/avdevice.dll", Path.Combine(FFmpegBinaryPath, "avdevice.dll"));
        RuntimeDependencies.Add("$(ProjectDir)/Binaries/Win64/avfilter.dll", Path.Combine(FFmpegBinaryPath, "avfilter.dll"));
        RuntimeDependencies.Add("$(ProjectDir)/Binaries/Win64/avformat.dll", Path.Combine(FFmpegBinaryPath, "avformat.dll"));
        RuntimeDependencies.Add("$(ProjectDir)/Binaries/Win64/avutil.dll", Path.Combine(FFmpegBinaryPath, "avutil.dll"));
        RuntimeDependencies.Add("$(ProjectDir)/Binaries/Win64/swresample.dll", Path.Combine(FFmpegBinaryPath, "swresample.dll"));
        RuntimeDependencies.Add("$(ProjectDir)/Binaries/Win64/swscale.dll", Path.Combine(FFmpegBinaryPath, "swscale.dll"));
        RuntimeDependencies.Add("$(ProjectDir)/Binaries/Win64/postproc.dll", Path.Combine(FFmpegBinaryPath, "postproc.dll"));

        #endregion

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
                "RenderCore",
                "RHI",
                "Projects",
                "MovieSceneCapture",
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

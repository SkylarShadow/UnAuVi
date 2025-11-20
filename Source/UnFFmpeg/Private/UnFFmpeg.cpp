// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnFFmpeg.h"
#include "Interfaces/IPluginManager.h"
#define LOCTEXT_NAMESPACE "FUnFFmpegModule"

void FUnFFmpegModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	const FString PluginDir = IPluginManager::Get().FindPlugin(TEXT("UnAuVi"))->GetBaseDir();
	FString LibraryPath;
#if WITH_FFMPEG

#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*PluginDir, TEXT("ThirdParty/FFmpeg/bin/x64"));
#endif
	
	FFmpegAvcodecHandle = FPlatformProcess::GetDllHandle(*(LibraryPath + TEXT("avcodec.dll")));
	FFmpegAvdeviceHandle = FPlatformProcess::GetDllHandle(*(LibraryPath + TEXT("avdevice.dll")));
	FFmpegAvfiliterHandle = FPlatformProcess::GetDllHandle(*(LibraryPath + TEXT("avfilter.dll")));
	FFmpegAvformatHandle = FPlatformProcess::GetDllHandle(*(LibraryPath + TEXT("avformat.dll")));
	FFmpegAvutilHandle = FPlatformProcess::GetDllHandle(*(LibraryPath + TEXT("avutil.dll")));
	FFmpegSwresampleHandle = FPlatformProcess::GetDllHandle(*(LibraryPath + TEXT("swresample.dll")));
	FFmpegSwscaleHandle = FPlatformProcess::GetDllHandle(*(LibraryPath + TEXT("swscale.dll")));
	FFmpegPostprocHandle = FPlatformProcess::GetDllHandle(*(LibraryPath + TEXT("postproc.dll")));
	//if (/*FFmpegHandle*/ FFmpegAvcodecHandle&& FFmpegAvdeviceHandle && FFmpegAvfiliterHandle&& FFmpegAvformatHandle && FFmpegAvutilHandle && FFmpegSwresampleHandle&& FFmpegSwscaleHandle)
	//{
	//	
	//}
	//else
	//{
	//	FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load ffmpeg library"));
	//}
#endif
}

void FUnFFmpegModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

#if WITH_FFMPEG
	if (FFmpegAvcodecHandle)
	{
		FPlatformProcess::FreeDllHandle(FFmpegAvcodecHandle);
		FFmpegAvcodecHandle = nullptr;
	}
	if (FFmpegAvdeviceHandle)
	{
		FPlatformProcess::FreeDllHandle(FFmpegAvdeviceHandle);
		FFmpegAvdeviceHandle = nullptr;
	}
	if (FFmpegAvfiliterHandle)
	{
		FPlatformProcess::FreeDllHandle(FFmpegAvfiliterHandle);
		FFmpegAvfiliterHandle = nullptr;
	}
	if (FFmpegAvformatHandle)
	{
		FPlatformProcess::FreeDllHandle(FFmpegAvformatHandle);
		FFmpegAvformatHandle = nullptr;
	}
	if (FFmpegAvutilHandle)
	{
		FPlatformProcess::FreeDllHandle(FFmpegAvutilHandle);
		FFmpegAvutilHandle = nullptr;
	}
	if (FFmpegSwresampleHandle)
	{
		FPlatformProcess::FreeDllHandle(FFmpegSwresampleHandle);
		FFmpegSwresampleHandle = nullptr;
	}
	if (FFmpegSwscaleHandle)
	{
		FPlatformProcess::FreeDllHandle(FFmpegSwscaleHandle);
		FFmpegSwscaleHandle = nullptr;
	}
#endif
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnFFmpegModule, UnFFmpeg)
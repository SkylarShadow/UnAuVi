// Copyright Epic Games, Inc. All Rights Reserved.
#include "UnOpenCV.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FUnOpenCVModule"

void FUnOpenCVModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	const FString PluginDir = IPluginManager::Get().FindPlugin(TEXT("UnAuVi"))->GetBaseDir();
	FString LibraryPath;

#if WITH_OPENCV
	LibraryPath = FPaths::Combine(*PluginDir, TEXT("Source/ThirdParty/OpenCV/lib/"));
	OpenCV_World_Handler = FPlatformProcess::GetDllHandle(*(LibraryPath + TEXT("opencv_world4120.dll"))); //¼ÓÔØOpenCV¾ä±ú
#endif
}

void FUnOpenCVModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
#if WITH_OPENCV
	if (OpenCV_World_Handler)
	{
		FPlatformProcess::FreeDllHandle(OpenCV_World_Handler); //ÊÍ·ÅOpenCV¾ä±ú
		OpenCV_World_Handler = nullptr;
	}
#endif
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnOpenCVModule, UnOpenCV)
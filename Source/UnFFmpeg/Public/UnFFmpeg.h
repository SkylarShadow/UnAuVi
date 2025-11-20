// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FUnFFmpegModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	//FFmpeg¾ä±ú
	void* FFmpegHandle; 

	void* FFmpegAvcodecHandle;
	void* FFmpegAvdeviceHandle;
	void* FFmpegAvfiliterHandle;
	void* FFmpegAvformatHandle;
	void* FFmpegAvutilHandle;
	void* FFmpegSwresampleHandle;
	void* FFmpegSwscaleHandle;
	void* FFmpegPostprocHandle;
};

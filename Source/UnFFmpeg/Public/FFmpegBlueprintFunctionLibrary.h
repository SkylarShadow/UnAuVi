// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FFmpegBlueprintFunctionLibrary.generated.h"

#if WITH_FFMPEG

extern "C"{
	#include "libavcodec/avcodec.h"
}

#endif
/**
 * 
 */
UCLASS()
class UNFFMPEG_API UFFmpegBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FFmpeg", DisplayName = "Get FFmpeg Version")
	static FString GetFFmpegVersion();
};

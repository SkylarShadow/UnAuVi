// Fill out your copyright notice in the Description page of Project Settings.


#include "FFmpegBlueprintFunctionLibrary.h"
FString UFFmpegBlueprintFunctionLibrary::GetFFmpegVersion()
{
	unsigned version = avcodec_version();
	UE_LOG(LogTemp, Warning, TEXT("FFmpeg avcodec_version = %d"), version);

	return FString();
}

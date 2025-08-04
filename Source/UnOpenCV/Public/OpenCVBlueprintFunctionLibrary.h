// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OpenCVBlueprintFunctionLibrary.generated.h"

#if WITH_OPENCV
#include "opencv2/core.hpp"   
#include "opencv2/opencv.hpp" 
#endif

/**
 * 
 */
UCLASS()
class UNOPENCV_API UOpenCVBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "OpenCV", DisplayName = "Get OpenCV Version")
	static FString GetOpenCVVersion();

	UFUNCTION(BlueprintCallable, Category = "OpenCV")
	static UTexture2D* CombineTexturesToAtlas(const TArray<UTexture2D*>& Textures,const FString& SaveDirectory);

	static cv::Mat ConvertTextureToMat(UTexture2D* Texture2D);

};

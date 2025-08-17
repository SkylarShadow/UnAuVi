// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include "TextureBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TEXTUREUTILS_API UTextureBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	// load Image(png,jpeg,bmp etc) from path
	UFUNCTION(BlueprintCallable,Category = "TextureUtils")
	static UTexture2D* LoadImage(const FString& InLoadPath);

	UFUNCTION(BlueprintCallable, Category = "TextureUtils")
	static UTexture2D* CopyTexture2D(UTexture2D* InTexture, UObject* Outer, FName Name);

	UFUNCTION(BlueprintCallable, Category = "TextureUtils")
	static void SaveTextureToAsset(UTexture2D* InTexture);

	UFUNCTION(BlueprintCallable, Category = "TextureUtils")
	static FSoftObjectPath SaveTextureToAssetInDir(UTexture2D* InTexture, const FString& InSavePath, const FString& InAssetName);

	FORCEINLINE static bool IsPowerOfTwo(int32 Value)
	{
		return Value > 0 && ((Value & (Value - 1)) == 0);
	}

	UFUNCTION(BlueprintCallable, Category = "TextureUtils")
	static bool CheckTextureSize(UTexture2D* Texture);

	UFUNCTION(BlueprintCallable, Category = "TextureUtils")
	static TArray<UTexture2D*> GetTexturesInFolder(const FString& FolderPath);

	UFUNCTION(BlueprintCallable, Category = "TextureUtils")
	static void CheckTexturesSizeInFolder(const FString& FolderPath);

	static UTexture2D* SaveTexture_Internal(UTexture2D* InTexture,const FString& PackageName,const FString& AssetName);
};

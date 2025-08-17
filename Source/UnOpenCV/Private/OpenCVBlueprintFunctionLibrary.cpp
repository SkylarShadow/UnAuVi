// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenCVBlueprintFunctionLibrary.h"
#include "Modules/ModuleManager.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Misc/FileHelper.h"
#include "Engine/Texture2D.h"
#include "Engine/Texture.h"
#include "Engine/TextureMipDataProviderFactory.h"
#include "RenderUtils.h"
#include "ImageUtils.h"
#include "AssetToolsModule.h"
#include "Factories/TextureFactory.h"
#include "ObjectTools.h"
#include "PackageTools.h"
#include "AssetRegistry/AssetRegistryModule.h"


FString UOpenCVBlueprintFunctionLibrary::GetOpenCVVersion()
{
	const auto OpenCVVersion = cv::getVersionString();
	return *FString(OpenCVVersion.c_str());
}


cv::Mat UOpenCVBlueprintFunctionLibrary::ConvertTextureToMat(UTexture2D* Texture2D)
{
	cv::Mat OutMat;
	if (!Texture2D || !Texture2D->GetPlatformData() || Texture2D->GetPlatformData()->Mips.Num() == 0)
	{
		UE_LOG(LogTexture, Error, TEXT("invaild PlatformData or Mipmap"));
		return OutMat;
	}

	// save old texture settings
	const TextureCompressionSettings OldCompressionSettings = Texture2D->CompressionSettings;
	const TextureMipGenSettings OldMipGenSettings = Texture2D->MipGenSettings;
	const bool OldSRGB = Texture2D->SRGB;


	Texture2D->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	Texture2D->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	Texture2D->SRGB = false;
	Texture2D->UpdateResource();

	FTexture2DMipMap& Mip = Texture2D->GetPlatformData()->Mips[0];
	const int32 Width = Texture2D->GetPlatformData()->SizeX;
	const int32 Height = Texture2D->GetPlatformData()->SizeY;

	uint8* Data = (uint8*)Mip.BulkData.Lock(LOCK_READ_ONLY);
	if (!Data)
	{
		Mip.BulkData.Unlock();
		UE_LOG(LogTexture, Error, TEXT("can't lock bulkdata"));
		return OutMat;
	}

	OutMat = cv::Mat(Height, Width, CV_8UC4); 
	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			int32 PixelIndex = (y * Width + x) * 4;
			uint8 B = Data[PixelIndex + 0];
			uint8 G = Data[PixelIndex + 1];
			uint8 R = Data[PixelIndex + 2];
			uint8 A = Data[PixelIndex + 3]; 

			cv::Vec4b Color(B, G, R, A);
			/*
			// sRGB handle
			if (OldSRGB)
			{
				auto ToLinear = [](uint8 c) -> uint8 {
					float cf = c / 255.0f;
					cf = FMath::Pow(cf, 2.2f);   // gamma expand
					return (uint8)FMath::Clamp(cf * 255.0f, 0.0f, 255.0f);
					};
				Color[0] = ToLinear(Color[0]); // B
				Color[1] = ToLinear(Color[1]); // G
				Color[2] = ToLinear(Color[2]); // R
			}

			// Handle compression setting (example: normal map)
			if (OldCompressionSettings == TC_Normalmap)
			{
				// Unreal encodes normals in BC5 as XY in RG
				// Z is reconstructed
				FVector2D xy(
					(Color[2] / 255.0f) * 2.0f - 1.0f, // R
					(Color[1] / 255.0f) * 2.0f - 1.0f  // G
				);
				float z = FMath::Sqrt(FMath::Clamp(1.0f - xy.X * xy.X - xy.Y * xy.Y, 0.0f, 1.0f));
				// pack back into color
				Color[2] = (uint8)((xy.X * 0.5f + 0.5f) * 255.0f);
				Color[1] = (uint8)((xy.Y * 0.5f + 0.5f) * 255.0f);
				Color[0] = (uint8)((z * 0.5f + 0.5f) * 255.0f);
				Color[3] = 255;
			}
			*/
			OutMat.at<cv::Vec4b>(y, x) = Color;
		}
	}

	Mip.BulkData.Unlock();

	// restore old texture settings
	Texture2D->CompressionSettings = OldCompressionSettings;
	Texture2D->MipGenSettings = OldMipGenSettings;
	Texture2D->SRGB = OldSRGB;
	Texture2D->UpdateResource();

	return OutMat;
}

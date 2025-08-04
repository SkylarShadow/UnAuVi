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

			OutMat.at<cv::Vec4b>(y, x) = cv::Vec4b(B,G,R,A);
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


UTexture2D* UOpenCVBlueprintFunctionLibrary::CombineTexturesToAtlas(const TArray<UTexture2D*>& Textures, const FString& SaveDirectory)
{
	if (Textures.Num() == 0) return nullptr;

	// 转mat
	TArray<cv::Mat> Mats;
	int TotalWidth = 0;
	int CommonHeight = 0;

	for (UTexture2D* Texture : Textures)
	{
		cv::Mat MatImage = ConvertTextureToMat(Texture);
		if (!MatImage.empty())
		{
			CommonHeight = MatImage.rows;
			TotalWidth += MatImage.cols;
			Mats.Add(MatImage);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to convert texture to Mat."));
		}
	}

	if (Mats.Num() == 0) return nullptr;

	// Atlas
	cv::Mat Atlas;
	std::vector<cv::Mat> MatsVector(Mats.GetData(), Mats.GetData() + Mats.Num());
	cv::hconcat(MatsVector, Atlas);

	const int Width = Atlas.cols;
	const int Height = Atlas.rows;


	UTexture2D* AtlasTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
	AtlasTexture->MipGenSettings = TMGS_Sharpen4; //TODO: expose to param
	AtlasTexture->SRGB = false;
	AtlasTexture->CompressionSettings = TC_EditorIcon;
	FTexture2DMipMap& NewMip = AtlasTexture->GetPlatformData()->Mips[0];

	void* TextureData = NewMip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, Atlas.data, Width * Height * 4);
	NewMip.BulkData.Unlock();
	AtlasTexture->UpdateResource();

	return AtlasTexture;
}




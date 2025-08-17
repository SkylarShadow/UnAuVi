// Fill out your copyright notice in the Description page of Project Settings.


#include "TextureBlueprintFunctionLibrary.h"
#include "Engine/Texture2D.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/UObjectGlobals.h"
#include "Factories/TextureFactory.h"
#include "IContentBrowserSingleton.h"
#include "EditorDirectories.h"
#include "ObjectTools.h"
#include "ContentBrowserModule.h"
#include "Internationalization/Internationalization.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "ImageCoreUtils.h"
#include "ImageCore.h"

#define LOCTEXT_NAMESPACE "TexturesUtil"
DEFINE_LOG_CATEGORY_STATIC(LogTextureUtils, Log, All);


UTexture2D* UTextureBlueprintFunctionLibrary::LoadImage(const FString& InLoadPath)
{
#if WITH_EDITOR
	TArray64<uint8> Buffer;
	if (!FFileHelper::LoadFileToArray(Buffer, *InLoadPath))
	{
		return nullptr;
}

	const FString TextureName;
	const FString Extension = FPaths::GetExtension(InLoadPath).ToLower();
	const uint8* BufferPtr = Buffer.GetData();

	auto TextureFact = NewObject<UTextureFactory>();
	UTexture2D* OutTexture = Cast<UTexture2D>(TextureFact->FactoryCreateBinary(
		UTexture2D::StaticClass(), GetTransientPackage(), *TextureName, RF_Transient,
		NULL, *Extension, BufferPtr, BufferPtr + Buffer.Num(), GWarn));

	return OutTexture;

#else

	FImage ImageInfo;
	FImageUtils::LoadImage(*InLoadPath, ImageInfo);
	return FImageUtils::CreateTexture2DFromImage(ImageInfo);
#endif
}

UTexture2D* UTextureBlueprintFunctionLibrary::CopyTexture2D(UTexture2D* InTexture, UObject* Outer, FName Name)
{

	void* SrcMipData = InTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_ONLY);
	const int32 InSizeX = InTexture->GetSizeX();
	const int32 InSizeY = InTexture->GetSizeY();
	const EPixelFormat InPixelFormat = InTexture->GetPixelFormat();
	const ERawImageFormat::Type InRawImageFormat = FImageCoreUtils::GetRawImageFormatForPixelFormat(InPixelFormat);
	const FImageView SrcMipImage(
		SrcMipData, InSizeX, InSizeY, 1, InRawImageFormat, InTexture->GetGammaSpace());
	EObjectFlags Flags = RF_Public | RF_Standalone | RF_Transactional;

	// create dst texture
	UTexture2D* NewTexture = NewObject<UTexture2D>(Outer, Name, Flags);
	NewTexture->SetPlatformData(new FTexturePlatformData());
	NewTexture->GetPlatformData()->SizeX = InSizeX;
	NewTexture->GetPlatformData()->SizeY = InSizeY;
	NewTexture->GetPlatformData()->SetNumSlices(1);
	NewTexture->GetPlatformData()->PixelFormat = InPixelFormat;

	// Allocate first mipmap.
	int32 NumBlocksX = InSizeX / GPixelFormats[InPixelFormat].BlockSizeX;
	int32 NumBlocksY = InSizeY / GPixelFormats[InPixelFormat].BlockSizeY;
	FTexture2DMipMap* Mip = new FTexture2DMipMap();
	Mip->SizeX = InSizeX;
	Mip->SizeY = InSizeY;
	//Mip->SizeX = 1;
	NewTexture->GetPlatformData()->Mips.Add(Mip);
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	Mip->BulkData.Realloc((int64)NumBlocksX * NumBlocksY * GPixelFormats[InPixelFormat].BlockBytes);
	Mip->BulkData.Unlock();

	// dst texture ImageView
	uint8* DstMipData = static_cast<uint8*>(NewTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
	const FImageView DstMipImage(
		DstMipData, InSizeX, InSizeY, 1, InRawImageFormat, InTexture->GetGammaSpace());

	// run CopyImage
	FImageCore::CopyImage(SrcMipImage, DstMipImage);

#if WITH_EDITORONLY_DATA
	NewTexture->Source.Init(
		InSizeX, InSizeY, 1, 1,
		FImageCoreUtils::ConvertToTextureSourceFormat(InRawImageFormat), DstMipData);
#endif

	// cleanup
	InTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
	NewTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
	NewTexture->UpdateResource();

	return NewTexture;
}


UTexture2D* UTextureBlueprintFunctionLibrary::SaveTexture_Internal(UTexture2D* InTexture, const FString& PackageName, const FString& AssetName)
{
	if (!InTexture) return nullptr;

	// Package FileName
	FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());

	// New Package
	UPackage* NewPackage = CreatePackage(*PackageName);
	NewPackage->FullyLoad();

	// Copy Texture
	UTexture2D* NewTexture = CopyTexture2D(InTexture, NewPackage, FName(*AssetName));

	// Thumbnail
	FObjectThumbnail NewThumbnail;
	ThumbnailTools::RenderThumbnail(
		NewTexture, NewTexture->GetSizeX(), NewTexture->GetSizeY(),
		ThumbnailTools::EThumbnailTextureFlushMode::NeverFlush, nullptr,
		&NewThumbnail);
	ThumbnailTools::CacheThumbnail(NewTexture->GetFullName(), &NewThumbnail, NewPackage);

	// Register + Mark dirty
	NewPackage->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(NewTexture);

	// Save
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	SaveArgs.bWarnOfLongFilename = true;

	if (!UPackage::SavePackage(NewPackage, NewTexture, *PackageFileName, SaveArgs))
	{
		UE_LOG(LogTextureUtils, Error, TEXT("Failed to save asset: %s"), *PackageFileName);
		return nullptr;
	}

	return NewTexture;
}

void UTextureBlueprintFunctionLibrary::SaveTextureToAsset(UTexture2D* InTexture)
{
	if (!InTexture) return;

	// open save asset dialog, choose where/which to save
	FSaveAssetDialogConfig SaveAssetDialogConfig;

	SaveAssetDialogConfig.DefaultPath = FEditorDirectories::Get().GetLastDirectory(ELastDirectory::NEW_ASSET);
	SaveAssetDialogConfig.AssetClassNames.Add(UTexture2D::StaticClass()->GetClassPathName());
	SaveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;
	SaveAssetDialogConfig.DialogTitleOverride = LOCTEXT("SaveTextureAsAssetTitle", "SaveTextureAsAsset");

	const FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	const FString SaveObjectPath = ContentBrowserModule.Get().CreateModalSaveAssetDialog(SaveAssetDialogConfig);

	if (SaveObjectPath.IsEmpty()) return;

	// init save info
	const FString PackageName = FPackageName::ObjectPathToPackageName(SaveObjectPath);
	const FString TextureName = FPaths::GetBaseFilename(PackageName);

	SaveTexture_Internal(InTexture, PackageName, TextureName);

	FEditorDirectories::Get().SetLastDirectory(ELastDirectory::NEW_ASSET, FPaths::GetPath(PackageName));
}

FSoftObjectPath UTextureBlueprintFunctionLibrary::SaveTextureToAssetInDir(UTexture2D* InTexture, const FString& InSavePath, const FString& InAssetName)
{
	if (!InTexture) return FSoftObjectPath();

	// LongPackageName ( /Game/...)
	FString PackageName = InSavePath / InAssetName;
	FPaths::NormalizeFilename(PackageName);

	UTexture2D* NewTexture = SaveTexture_Internal(InTexture, PackageName, InAssetName);
	if (!NewTexture) return FSoftObjectPath();

	// 返回软引用路径
	return FSoftObjectPath(NewTexture);
}



bool UTextureBlueprintFunctionLibrary::CheckTextureSize(UTexture2D* Texture)
{
	int32 Width = Texture->GetSizeX();
	int32 Height = Texture->GetSizeY();

	bool bWidthIsPow2 = IsPowerOfTwo(Width);
	bool bHeightIsPow2 = IsPowerOfTwo(Height);
	bool bSquare = Width == Height;

	if (!bWidthIsPow2 || !bHeightIsPow2 || !bSquare)
	{
		FString Issues;
		if (!bWidthIsPow2 || !bHeightIsPow2)
		{
			Issues += TEXT("the size is not a power of 2.");
		}
		if (!bSquare)
		{
			if (!Issues.IsEmpty()) Issues += TEXT("，");
			Issues += TEXT("no squre texture");
		}

		UE_LOG(LogTexture, Warning, TEXT("Textures: %s : %s (size: %dx%d)"),
			*Texture->GetName(), *Issues, Width, Height);

		return false;
	}
	return true;
}


TArray<UTexture2D*> UTextureBlueprintFunctionLibrary::GetTexturesInFolder(const FString& FolderPath)
{
	TArray<UTexture2D*> TextureArray;

	if (FolderPath.IsEmpty())
	{
		UE_LOG(LogTexture, Warning, TEXT("null FolderPath"));
		return TextureArray;
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	FARFilter Filter;
	Filter.PackagePaths.Add(*FolderPath);
	Filter.bRecursivePaths = true;
	Filter.ClassPaths.Add(UTexture2D::StaticClass()->GetClassPathName());

	TArray<FAssetData> AssetList;
	AssetRegistryModule.Get().GetAssets(Filter, AssetList);

	for (const FAssetData& AssetData : AssetList)
	{
		if (UTexture2D* Texture = Cast<UTexture2D>(AssetData.GetAsset()))
		{
			TextureArray.Add(Texture);
		}
	}

	return TextureArray;
}

void UTextureBlueprintFunctionLibrary::CheckTexturesSizeInFolder(const FString& FolderPath)
{
	TArray<UTexture2D*> Textures = GetTexturesInFolder(FolderPath);

	if (Textures.Num() == 0)
	{
		UE_LOG(LogTexture, Warning, TEXT("cant found textures under %s "), *FolderPath);
		return;
	}


	const int32 Total = Textures.Num();
	FScopedSlowTask SlowTask(Total, FText::FromString(TEXT("check texture size(whether is power of 2)")));
	SlowTask.MakeDialog();

	for (int32 i = 0; i < Total; ++i)
	{
		const FAssetData& AssetData = Textures[i];
		const FString TextureName = AssetData.AssetName.ToString();

		float ProgressPercent = (float(i + 1) / float(Total)) * 100.f;
		UE_LOG(LogTextureUtils, Log, TEXT("[%3.0f%%] check textures：%s"), ProgressPercent, *TextureName);

		SlowTask.EnterProgressFrame(1.f, FText::Format(
			NSLOCTEXT("TextureCheck", "Progress", "check textures {0} / {1} :{2}"),
			FText::AsNumber(i + 1),
			FText::AsNumber(Total),
			FText::FromString(TextureName)));

		if (UTexture2D* Texture = Cast<UTexture2D>(AssetData.GetAsset()))
		{
			CheckTextureSize(Texture);
		}
	}

	UE_LOG(LogTextureUtils, Log, TEXT("Check completed, a total of %d textures detected."), Textures.Num());
}
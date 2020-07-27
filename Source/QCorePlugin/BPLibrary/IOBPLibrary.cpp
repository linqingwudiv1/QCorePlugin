// Fill out your copyright notice in the Description page of Project Settings.

#include "IOBPLibrary.h"


#include "Misc/FileHelper.h"
// 
#include "Helper/ImageHelper.h"

UIOBPLibrary::UIOBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UTexture2D * UIOBPLibrary::LoadTexture2DFromDisk(const FString &Path, bool  bAutoGenerateMips, bool bForceGenerateMips)
{
	return UImageHelper::LoadFromDisk(Path, bAutoGenerateMips, bForceGenerateMips);
}

bool UIOBPLibrary::WriteFile(const FString& FilePath, TArray<uint8>& Data)
{
	if (!FPaths::FileExists(FilePath)) 
	{
		FFileHelper::SaveArrayToFile(Data, *FilePath);
	}
	else 
	{
		FArchive* FileWriter = IFileManager::Get().CreateFileWriter(*FilePath, EFileWrite::FILEWRITE_Append);
		FileWriter->Serialize(Data.GetData(), Data.Num());
		FileWriter->Close();
	}
	
	return true;
}

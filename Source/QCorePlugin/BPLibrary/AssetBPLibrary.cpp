// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetBPLibrary.h"

#include "Engine/AssetManager.h"
#include "Engine/Engine.h"

UAssetBPLibrary::UAssetBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FString UAssetBPLibrary::GetSoftObjectPtrRefPath(TSoftObjectPtr<class USkeletalMesh> ptr)
{
	FSoftObjectPath objPath = ptr.ToSoftObjectPath();
	return objPath.GetAssetPathString();
}

UObject* UAssetBPLibrary::LoadAsset(UObject* WorldContextObject, TSubclassOf<UObject> ObjectClass, const FString& Path)
{
	UObject* obj = LoadObject<UObject>(nullptr, *Path);

	if ( obj )
	{
		UE_LOG(LogTemp, Error, TEXT("Static Object name is %s"), *obj->GetName());
	}
	
	return obj;
}

bool UAssetBPLibrary::IsValidOfAsset(UObject* WorldContextObject, const FString& Path)
{

	FSoftObjectPath SoftPath (Path);
	FAssetData AssetData;

	UAssetManager &AssetMgr = UAssetManager::Get();
	AssetMgr.GetAssetDataForPath(SoftPath, AssetData);


	return AssetData.GetPrimaryAssetId().IsValid();

}


bool UAssetBPLibrary::IsLoadedOfAsset(UObject* WorldContextObject, const FString& Path)
{
	FSoftObjectPath SoftPath(Path);
	FAssetData AssetData;

	UAssetManager &AssetMgr = UAssetManager::Get();
	AssetMgr.GetAssetDataForPath(SoftPath, AssetData);

	return AssetData.IsAssetLoaded();
}
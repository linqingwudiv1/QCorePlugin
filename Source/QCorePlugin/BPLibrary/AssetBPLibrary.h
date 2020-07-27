// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "AssetBPLibrary.generated.h"

/**
 * 
 */
UCLASS()
class QCOREPLUGIN_API UAssetBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	
	UFUNCTION( BlueprintPure, Category = "QCore | Asset")
		/** 获取Skeletal Mesh Asset 路径(目前主要用于编辑器工具集中) */
		static FString GetSoftObjectPtrRefPath(TSoftObjectPtr<class USkeletalMesh> ptr);

	UFUNCTION(BlueprintPure, Category = "QCore | Asset", meta = ( HidePin = "WorldContextObject", DeterminesOutputType = "ObjectClass", DefaultToSelf = "WorldContextObject") )
		/**便捷函数,加载资源并自动转换资源为指定类型 */
		static UObject* LoadAsset(UObject* WorldContextObject, TSubclassOf<UObject> ObjectClass, const FString &Path  );
		
	UFUNCTION(BlueprintPure, Category = "QCore | Asset", meta = (HidePin = "WorldContextObject"))
		static bool IsValidOfAsset(UObject* WorldContextObject, const FString &Path);

	UFUNCTION(BlueprintPure, Category = "QCore | Asset", meta = (HidePin = "WorldContextObject"))
		static bool IsLoadedOfAsset(UObject * WorldContextObject, const FString & Path);
};

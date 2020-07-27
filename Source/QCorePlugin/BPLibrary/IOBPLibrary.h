// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "IOBPLibrary.generated.h"

class UTexture2D;
class UTexture2DDynamic;


/**
 * 
 */
UCLASS()
class QCOREPLUGIN_API UIOBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:

	/**
	 * 从磁盘中加载UTexture2D
	 **/
	UFUNCTION(BlueprintPure, Category = "QCore | IO |Common Function")
		static class UTexture2D* LoadTexture2DFromDisk(const FString &Path,  bool  bAutoGenerateMips = true, bool bForceGenerateMips = false);


		static bool WriteFile(const FString& FilePath, TArray<uint8>& Data);
};

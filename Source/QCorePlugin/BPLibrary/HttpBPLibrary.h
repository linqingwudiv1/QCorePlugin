// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "HttpBPLibrary.generated.h"

class UDownloadHelper;
/**
 * 
 */
UCLASS()
class QCOREPLUGIN_API UHttpBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "QCore | Http", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject") )
    static UDownloadHelper* DownloadRange(UObject* WorldContextObject, const FString & Url);
};

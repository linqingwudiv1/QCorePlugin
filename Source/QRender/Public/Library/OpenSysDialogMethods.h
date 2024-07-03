// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OpenSysDialogMethods.generated.h"


UENUM(BlueprintType)
enum class ESystemFileDialogTypeEx :  uint8
{
	Save,
	Open
};

//DECLARE_ENUM_TO_STRING(ESystemFileDialogType);
/**
 * 版本更新相关,GameIsntance检查或服务器socket主动进行新版本推送
 */
UCLASS()
class QRENDER_API UOpenSysDialogMethods : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "PuertsExtra | Library")
	static bool OpenSystemFileDialog( 
									  FString& OutFilePath, 
									  const FString& Title, const FString& DefaultPath, 
									  ESystemFileDialogTypeEx Type = ESystemFileDialogTypeEx::Open, 
									  const FString& Filter = TEXT("*.*")
								     );

};

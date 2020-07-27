// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QUI_TabItem.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct QUMG_API FQUI_TabItem
{
	GENERATED_USTRUCT_BODY()

public:
	/**  */
	FQUI_TabItem()
	{
	
	}

public:
	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CppVariant")
		FString Name;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CppVariant")
		FString ImgPath;

};
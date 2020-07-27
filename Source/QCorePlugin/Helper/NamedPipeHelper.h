// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Core/Public/HAL/PlatformNamedPipe.h"

#include "NamedPipeHelper.generated.h"
/**
 * 
 */
UCLASS(ABSTRACT)
class QCOREPLUGIN_API UNamedPipeHelper: public UObject
{
	GENERATED_BODY()
protected:
	/**  */
	UNamedPipeHelper();

public:
	static UNamedPipeHelper* CreateNamedPipeHelper( const FString& PipeName = TEXT("\\\\.\\my_pipe"),
													bool bServer = false, 
													bool bAsync = true );

	/** 返回pipe管线 */
	FPlatformNamedPipe* GetNamedPipe();

protected:

	/**  */
	FPlatformNamedPipe NamedPipe;

	FString PipeName;
	bool bServer = false;
	bool bAsync = true;
};

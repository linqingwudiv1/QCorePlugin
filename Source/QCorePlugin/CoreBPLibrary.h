// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"


#include "PlatformNamedPipe.h"

#include "CoreBPLibrary.generated.h"


/**
 * 
 */
UCLASS()
class QCOREPLUGIN_API UCoreBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:

	/** 正则 */
	UFUNCTION(BlueprintPure, Category = "Base|Regex")
		static bool RegexMatch(const FString& Str, const FString& Pattern, TArray<FString>& Result);


	UFUNCTION(BlueprintCallable, Category = "Core | Common Function", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static TArray<FString> OpenFileDialog(UObject * WorldContextObject);

	/**
	 * 模型的材质数
	 **/
	UFUNCTION(BlueprintCallable, Category = "Core | Common Function")
		static int32 GetMeshMaterialNum(class UStaticMesh *mesh);
	
	/** Shell */
	UFUNCTION(BlueprintCallable, Category = "Core | Common Function")
		static bool executeShellCMD(const FString& cmd);

	UFUNCTION(BlueprintCallable, Category = "Core | Common Function", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static FString ReadFromIPC(UObject * WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Core | Common Function", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static int CreateIPC(UObject * WorldContextObject);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Core | Common Function", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool WriteFromIPC(UObject * WorldContextObject);

	/** Pipe是否可读写 */
	UFUNCTION(BlueprintPure, Category = "Core | Common Function", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool bIsRW(UObject * WorldContextObject);

	static FPlatformNamedPipe* GetNamedPipe(const FString& PipeName = TEXT("\\\\.\\my_pipe"), bool bServer = false, bool bAsync = true) ;

	/** 解码URL */
	UFUNCTION(BlueprintPure, Category = "Core | Common Function")
		static FString UrlEncode( const FString& UnencodedString);
};

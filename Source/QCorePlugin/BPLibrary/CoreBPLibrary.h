// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"


#include "PlatformNamedPipe.h"

#include "CoreBPLibrary.generated.h"


UENUM(BlueprintType)
enum class  EM_EFileDialogFlags : uint8
{
	None = 0x00		UMETA(DisplayName = "None"), // No flags
	Multiple = 0x01 UMETA(DisplayName = "Multiple") // Allow multiple file selections
};

/**
 * 
 */
UCLASS()
class QCOREPLUGIN_API UCoreBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:

	/** 正则 */
	UFUNCTION(BlueprintPure, Category = "QCore |Common Function")
		static bool RegexMatch(const FString& Str, const FString& Pattern, TArray<FString>& Result);

	/**
	 *
	 **/
	UFUNCTION(BlueprintCallable, Category = "QCore | Common Function", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static TArray<FString> OpenFileDialog(UObject * WorldContextObject,
											  const FString &Title = "Title",
											  const FString defPath = "", 
											  const FString& FileTypes = "All Files|*.*", 
											  EM_EFileDialogFlags Flags = EM_EFileDialogFlags::None);


	/**
	* 从磁盘中加载UTexture2D 
	**/
	UFUNCTION(BlueprintPure, Category = "QCore |Common Function")
		static class UTexture2D* LoadTexture2DFromDisk(const FString &Path,  bool  bAutoGenerateMips = true, bool bForceGenerateMips = false);
	
	/**
	 * 模型的材质数
	 **/
	UFUNCTION(BlueprintCallable, Category = "QCore | Common Function")
		static int32 GetMeshMaterialNum(class UStaticMesh *mesh);
	
	/** Shell */
	UFUNCTION(BlueprintCallable, Category = "QCore | Common Function")
		static bool executeShellCMD(const FString& cmd);
	
	/**
	 *
	 **/
	UFUNCTION(BlueprintCallable, Category = "QCore | Common Function", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static FString ReadFromIPC(UObject * WorldContextObject);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "QCore | Common Function", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool WriteFromIPC(UObject * WorldContextObject);

	/** Pipe是否可读写 */
	UFUNCTION(BlueprintPure, Category = "QCore | Common Function", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool bIsRW(UObject * WorldContextObject);

	static FPlatformNamedPipe* GetNamedPipe(const FString& PipeName = TEXT("\\\\.\\my_pipe"), bool bServer = false, bool bAsync = true) ;

	/** 解码URL */
	UFUNCTION(BlueprintPure, Category = "QCore | Common Function")
		static FString UrlEncode( const FString& UnencodedString);

	/** 改变3D视口大小*/
	UFUNCTION(BlueprintCallable, Category = "QCore | Common Function", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static void AdjustViewportSize(UObject * WorldContextObject, FMargin padding);

	/// 获取Skeletal Mesh Asset路径（目前主要用于编辑器工具集中）
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "QCore | Common Function")
		static FString GetSoftObjectPtrRefPath(TSoftObjectPtr<class USkeletalMesh> ptr);

};

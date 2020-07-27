// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "CoreBPLibrary.generated.h"

class UTexture2D;
class UTexture2DDynamic;
class UObject;

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

	UFUNCTION(BlueprintPure, Category = "QCore |Common Function")
		static FString GetPointerAddress(UObject * obj);


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
	 * 模型的材质数
	 **/
	UFUNCTION(BlueprintCallable, Category = "QCore | Common Function")
		static int32 GetMeshMaterialNum(class UStaticMesh *mesh);
	
	/** Shell */
	UFUNCTION(BlueprintCallable, Category = "QCore | Common Function")
		static bool executeShellCMD(const FString& cmd);
	
	/** 解码URL */
	UFUNCTION(BlueprintPure, Category = "QCore | Common Function")
		static FString UrlEncode( const FString& UnencodedString);

	/** 改变3D视口大小*/
	UFUNCTION(BlueprintCallable, Category = "QCore | Common Function", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static void AdjustViewportSize(UObject * WorldContextObject, FMargin padding);

	/**  */
	UFUNCTION(BlueprintPure,  Category = "QCore | Common Function")
		static  UTexture2D* ConvertTexture2DDynaimcToTexture2D(UTexture2DDynamic* target);

};

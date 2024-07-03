// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DTO/ImageFormatEx.h"
#include "CaptureExtensionMethods.generated.h"



class UMediaUserSettings;
class UScreenshotUserSettings;
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHasScreenshotCompleted, const FString&, SavePath);
/**
 * 版本更新相关,GameIsntance检查或服务器socket主动进行新版本推送
 */
UCLASS()
class QRENDER_API UCaptureExtensionMethods : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** Debug */
	UFUNCTION(BlueprintCallable)
	static void ScreenShot_HighRes();

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void HighScreenShot( UObject* WorldContextObject, EImageFormatEx InSaveType);

	static void CustomCameraHighScreenShot(
		UWorld* InWorld, EImageFormatEx InSaveType,
		const FVector& InLocation, const FRotator& InRotation, float InFov);

	static void CustomHighScreenShot(
		UWorld* InWorld, const FVector& InLocation, const FRotator& InRotation, float InFov,
		float InScreenPercent, const FVector2D& InSize,
		TFunction<void(const TArray64<uint8>&, const FVector2D&)> InCallback);

	UFUNCTION(BlueprintCallable)
	static UMediaUserSettings* GetMediaUserSettings();

	UFUNCTION(BlueprintCallable)
	static void SaveMediaUserSettings();

	UFUNCTION(BlueprintCallable)
	static UScreenshotUserSettings* GetScreenshotUserSettings();

	UFUNCTION(BlueprintCallable)
	static void SaveScreenshotUserSettings();

	/*截帧,用于相机位的UI封面 */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static UTexture2D* CaptureCameraFrame(UObject* WorldContextObject);


	UFUNCTION(BlueprintCallable)
	static void CubeMap2Per();
};
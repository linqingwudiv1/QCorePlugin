// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"


#include "CoordProjectionBPLibrary.generated.h"


/**
 * 2D-3D空间坐标系投影
 */
UCLASS()
class QCOREPLUGIN_API UCoordProjectionBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/* 世界坐标转屏幕坐标(屏幕投影) */
	UFUNCTION(BlueprintPure, Category = "QCore | CoordProjection")
		static FVector2D ProjectWorldToScreenPosition(const FVector& WorldLocation);

	/* 转换屏幕坐标到场景坐标(屏幕解投影). */
	UFUNCTION(BlueprintPure, Category = "QCore | CoordProjection", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool DeprojectScreenPosition(UObject* WorldContextObject, const FVector2D& ViewportPosition, FVector& OutWorldOrigin, FVector& OutWorldDirection);

	/** 世界坐标盒体到屏幕盒体(绘制线)的投影. */
	UFUNCTION(BlueprintPure, Category = "QCore | CoordProjection", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static TArray<FVector2D> WorldBoxToScreenBox(UObject *WorldContextObject,const FVector& Center, const FRotator& rot, const FVector& Extend);
};

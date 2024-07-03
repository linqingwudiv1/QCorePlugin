// Copyright Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	Renderer.cpp: Renderer module implementation.
=============================================================================*/

#include "CaptureScreenPercentageDriver.h"
#include "UnrealEngine.h"

#include "Misc/ConfigCacheIni.h"
#include "DynamicResolutionState.h"

FCaptureScreenPercentageDriver::FCaptureScreenPercentageDriver(
	const FSceneViewFamily& InViewFamily,
	float InGlobalResolutionFraction,
	float InGlobalResolutionFractionUpperBound)
	: ViewFamily(InViewFamily)
	  , GlobalResolutionFraction(InGlobalResolutionFraction)
	  , GlobalResolutionFractionUpperBound(InGlobalResolutionFractionUpperBound)
{
}

DynamicRenderScaling::TMap<float> FCaptureScreenPercentageDriver::GetResolutionFractionsUpperBound() const
{
	DynamicRenderScaling::TMap<float> UpperBounds;
	UpperBounds.SetAll(1.0f);

	if (ViewFamily.EngineShowFlags.ScreenPercentage)
	{
		UpperBounds[GDynamicPrimaryResolutionFraction] = FMath::Clamp(
			GlobalResolutionFractionUpperBound,
			ISceneViewFamilyScreenPercentage::kMinResolutionFraction,
			ISceneViewFamilyScreenPercentage::kMaxResolutionFraction);
	}

	return UpperBounds;
}

ISceneViewFamilyScreenPercentage* FCaptureScreenPercentageDriver::Fork_GameThread(
	const FSceneViewFamily& ForkedViewFamily) const
{
	check(IsInGameThread());
	//return nullptr;
	return new FCaptureScreenPercentageDriver(
		ForkedViewFamily, GlobalResolutionFraction, GlobalResolutionFractionUpperBound);
}

DynamicRenderScaling::TMap<float> FCaptureScreenPercentageDriver::GetResolutionFractions_RenderThread() const
{
	check(IsInRenderingThread());

	DynamicRenderScaling::TMap<float> ResolutionFractions;
	ResolutionFractions.SetAll(1.0f);

	if (ViewFamily.EngineShowFlags.ScreenPercentage)
	{
		ResolutionFractions[GDynamicPrimaryResolutionFraction] = FMath::Clamp(
			GlobalResolutionFraction,
			ISceneViewFamilyScreenPercentage::kMinResolutionFraction,
			ISceneViewFamilyScreenPercentage::kMaxResolutionFraction);
	}

	return ResolutionFractions;
}


//void FCaptureScreenPercentageDriver::ComputePrimaryResolutionFractions_RenderThread(
//	TArray<FSceneViewScreenPercentageConfig>& OutViewScreenPercentageConfigs) const
//{
//	check(IsInRenderingThread());
//	// 如果不支持ScenePercentage功能则直接返回
//	if (!ViewFamily.EngineShowFlags.ScreenPercentage)
//	{
//		return;
//	}
//
//	for (int32 i = 0; i < ViewFamily.Views.Num(); i++)
//	{
//		float ResolutionFraction = GlobalResolutionFraction;
//		//TODO:限制大小,避免卡死..后面可根据硬件等级设置上下界
//		OutViewScreenPercentageConfigs[i].PrimaryResolutionFraction = FMath::Clamp(
//			ResolutionFraction,
//			MinResolutionFraction,
//			MaxResolutionFraction );
//	}
//}

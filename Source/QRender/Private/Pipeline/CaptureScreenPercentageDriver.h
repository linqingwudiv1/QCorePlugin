// Copyright Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	RendererPrivate.h: Renderer interface private definitions.
=============================================================================*/

#pragma once

#include "SceneView.h"


/**
 * 自定义场景百分比接口...为了更自由的控制Capture的最大最小百分比
 */
class QRENDER_API FCaptureScreenPercentageDriver : public ISceneViewFamilyScreenPercentage
{
public:
	FORCEINLINE FCaptureScreenPercentageDriver(
		const FSceneViewFamily& InViewFamily,
		float InGlobalResolutionFraction)
		: FCaptureScreenPercentageDriver(InViewFamily, InGlobalResolutionFraction, InGlobalResolutionFraction)
	{
	}

	FCaptureScreenPercentageDriver(
		const FSceneViewFamily& InViewFamily,
		float InGlobalResolutionFraction,
		float InGlobalResolutionFractionUpperBound);

public:
	FORCEINLINE float GetGlobalResolutionFraction() const
	{
		return GlobalResolutionFraction;
	}

	FORCEINLINE bool SetGlobalResolutionFraction(float newFraction) volatile
	{
		float newF = this->GlobalResolutionFraction = FMath::Clamp(newFraction, ISceneViewFamilyScreenPercentage::kMinResolutionFraction, ISceneViewFamilyScreenPercentage::kMaxResolutionFraction);
		if (GlobalResolutionFractionUpperBound < newF)
		{
			//防止上界小于当前
			GlobalResolutionFractionUpperBound = this->GlobalResolutionFraction;
		}
		this->GlobalResolutionFraction = newF;
		return true;
	}

private:
	const FSceneViewFamily& ViewFamily;

	volatile float GlobalResolutionFraction;
	volatile float GlobalResolutionFractionUpperBound;

#pragma region ISceneViewFamilyScreenPercentage虚函数实现
	virtual DynamicRenderScaling::TMap<float> GetResolutionFractionsUpperBound() const override;
	virtual DynamicRenderScaling::TMap<float> GetResolutionFractions_RenderThread() const override;
	virtual ISceneViewFamilyScreenPercentage* Fork_GameThread(const class FSceneViewFamily& ForkedViewFamily) const override;
#pragma endregion
};

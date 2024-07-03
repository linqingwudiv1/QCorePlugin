#pragma once

#include "CoreMinimal.h"
#include "SceneViewExtension.h"


class FCaptureRTTViewExtension : public FSceneViewExtensionBase
{
public:
	FCaptureRTTViewExtension(const FAutoRegister& AutoReg);
	~FCaptureRTTViewExtension();

	// FSceneViewExtensionBase implementation : 
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override;

	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override
	{
	}

	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override
	{
	}

	virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override
	{
	}

	virtual void PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override;


	bool bRebuildGPUData = false;

	FBufferRHIRef DataBuffer;
	FShaderResourceViewRHIRef DataSRV;

	FBufferRHIRef IndirectionBuffer;
	FShaderResourceViewRHIRef IndirectionSRV;
};

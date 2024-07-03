#include "CaptureRTTViewExtension.h"

FCaptureRTTViewExtension::FCaptureRTTViewExtension(const FAutoRegister& AutoReg):
	FSceneViewExtensionBase(AutoReg)
{
}

FCaptureRTTViewExtension::~FCaptureRTTViewExtension()
{
}

void FCaptureRTTViewExtension::SetupViewFamily(FSceneViewFamily& InViewFamily)
{
}


void FCaptureRTTViewExtension::PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView)
{
}

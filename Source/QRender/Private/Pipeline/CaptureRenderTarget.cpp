// Fill out your copyright notice in the Description page of Project Settings.


#include "Pipeline/CaptureRenderTarget.h"

#include "Serialization/BufferArchive.h"
#include "CaptureScreenPercentageDriver.h"
#include "EngineModule.h"
#include "Misc/FileHelper.h"
#include "EngineUtils.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "Engine/PostProcessVolume.h"
#include "Components/PostProcessComponent.h"
#include "CanvasTypes.h"
#include "LegacyScreenPercentageDriver.h"
#if WITH_EDITOR
#include "AssetCompilingManager.h"
#endif

#include "OpenColorIODisplayExtension.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/PlayerController.h"
#pragma region warmup
#include "LandscapeProxy.h"
#include "DistanceFieldAtlas.h"
// #include "MeshCardRepresentation.h"
#define IS_ARTISTRENDERER 0
#if IS_ARTISTRENDERER
#include "DistanceFieldAtlas2.h"
#include "MeshCardRepresentation2.h"
#endif
#include "ShaderCompiler.h"
#pragma endregion warmup


#if WITH_OPENCV
#include "OpenCVHelper.h"
//OPENCV_INCLUDES_START
#include "PreOpenCVHeaders.h"
//#undef check // the check macro causes problems with opencv headers
#include "opencv2/opencv.hpp"
#include "PostOpenCVHeaders.h"
//OPENCV_INCLUDES_END
#endif

#include "Async/Async.h"

#include "ScreenRendering.h"
#include <Engine/World.h>


#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"


namespace CF_CaptureRenderTarget
{
	//默认的暖帧数
	static const uint8 DefaultWarmupFrameNum = 30;
	//默认预渲染帧数,Note:最低要求2帧...防止变换时模糊问题
	static const uint8 DefaultPreRenderingNum = 2;
}


UCaptureRenderTarget::UCaptureRenderTarget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SceneViewState.Allocate();
	{
	}
}

UCaptureRenderTarget::~UCaptureRenderTarget()
{
	FSceneViewStateInterface* Ref = SceneViewState.GetReference();
	if (Ref)
	{
		Ref->ClearMIDPool();
	}
	SceneViewState.Destroy();

	UE_LOG(LogTemp, Log, TEXT("UCaptureRenderTarget destroyed....."));
}

void UCaptureRenderTarget::Initilize(const FIntPoint& Size,
                                     const FVector& Location,
                                     const FRotator& Rotator,
                                     const float FOV,
                                     const float ScenePercentage,
                                     const bool InbNotInverseGamma,
                                     const bool bInAsync,
                                     APostProcessVolume* PP)
{
#if WITH_EDITOR
	check(Size.Size() > 0);
	check(FOV != 0);
#endif

	if(!InbNotInverseGamma)
	{
		this->Mat_PPGamma = LoadObject<UMaterial>(this, TEXT("/QCorePlugin/QRender/Materials/Mat_InverseGammaSpace.Mat_InverseGammaSpace"));
		//this->MatInsDyn_PPGamma = UMaterialInstanceDynamic::Create(Mat_PPGamma,this);
		//this->Mat_PPGamma = UMaterialInstanceDynamic::Create(Mat, this, TEXT("InverseGamma"));
	}

	this->mViewFOV = FOV;
	this->mViewLocation = Location;
	this->mViewRotation = Rotator;
	this->mScenePercentage = ScenePercentage;
	this->bNotInverseGamma = InbNotInverseGamma;
	this->mPP = PP;
	this->bAsync = bInAsync;

	if (this->mRTT2D != nullptr)
	{
		this->mRTT2D->ReleaseResource();
		this->mRTT2D = nullptr;
	}

	const FName TargetName = MakeUniqueObjectName(this, UTextureRenderTarget2D::StaticClass(), TEXT("SceneCaptureTextureTarget"));
	this->mRTT2D = NewObject<UTextureRenderTarget2D>(this, TargetName);

	check(this->mRTT2D != nullptr);
	InitRenderTarget(Size);

	this->CalculateProjectionMatrix();

	//this->SetupViewFamily();
	//this->SetupSceneView();
	this->SetState(ECaptureState::Warmup);
}

void UCaptureRenderTarget::SetScreenPercentage(float screenPercentage)
{
	this->mScenePercentage = screenPercentage;
}

void UCaptureRenderTarget::SetWorldPause(bool NewPause)
{
	if (this->bWorldPause != NewPause)
	{
		//this->UpdateSceneViewTime();
	}
	this->bWorldPause = NewPause;
}

void UCaptureRenderTarget::InitRenderTarget(const FIntPoint& Size)
{
	//TODO: rtt2d可以换成rhiTexture在renderthread处理.
	check(this->mRTT2D != nullptr);
	this->mRTT2D->ClearColor = FLinearColor::Transparent; //FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
	this->mRTT2D->InitCustomFormat(Size.X, Size.Y, EPixelFormat::PF_FloatRGBA, true);
	this->mRTT2D->TargetGamma = 2.2f;//FOpenColorIODisplayExtension::DefaultDisplayGamma; // default 2.2
	this->mRTT2D->bAutoGenerateMips = false;
	this->mRTT2D->Filter = TF_Default;//TF_Nearest;//TF_Trilinear;
	this->mRTT2D->AddressX = TA_Clamp;
	this->mRTT2D->AddressY = TA_Clamp;
	this->mRTT2D->MipsAddressU = TA_Clamp;
	this->mRTT2D->MipsAddressV = TA_Clamp;
	this->mRTT2D->UpdateResourceImmediate(true);
	this->mRTT2D->SRGB = true;
}

void UCaptureRenderTarget::ResizeRenderTarget(const FIntPoint& Size)
{
	this->mRTT2D->ResizeTarget(Size.X, Size.Y);
}

void UCaptureRenderTarget::SetState(ECaptureState NewState)
{
	if (this->CaptureState == NewState)
	{
		//UE_LOG(LogTemp, Error, TEXT("当前状态不应该等于新状态"));
		return;
	}

	//处理状态变换前的判断,断言确保状态从指定的状态切换过来.
	switch (NewState)
	{
		case ECaptureState::Warmup:
			{
				check(this->CaptureState == ECaptureState::Initilize);
				FlushAsyncEngineSystems();
				break;
			}
		case ECaptureState::Renderering:
			{
				check(this->CaptureState != ECaptureState::Initilize);
				break;
			}
		case ECaptureState::Renderered:
			{
				check(this->CaptureState == ECaptureState::Renderering);
				PreRenderingFrameNum = CF_CaptureRenderTarget::DefaultPreRenderingNum;
				this->bStartRender = false;
				break;
			}
		case ECaptureState::Captured:
			{
				check(this->CaptureState == ECaptureState::Renderered);
				break;
			}
		default:
			{
				break;
			}
	}

	this->CurPreFrame = 0;
	this->CaptureState = NewState;
}

TSharedPtr<FSceneViewFamilyContext> UCaptureRenderTarget::SetupViewFamily()
{
	check(GetRTT2D() != nullptr);
	check(GetWorld());

	UWorld* InWorld = GetWorld();
	FSceneInterface* Scene = InWorld->Scene;

	FTextureRenderTargetResource* RenderTarget = this->mRTT2D->GameThread_GetRenderTargetResource();
	EViewModeIndex ViewLightMode = VMI_Lit;
	FEngineShowFlags ShowFlags = GEngine->GameViewport->EngineShowFlags; ///**/FEngineShowFlags(ESFIM_Game);
	ShowFlags.SetScreenPercentage(true);
	TSharedPtr<FSceneViewFamilyContext> InViewFamily = MakeShared<FSceneViewFamilyContext>(
		(
			FSceneViewFamily::ConstructionValues(RenderTarget, Scene, ShowFlags)
		)
		.SetResolveScene(true)
		.SetWorldTimes( InWorld->GetTimeSeconds(),
						InWorld->GetDeltaSeconds(),
		                InWorld->GetRealTimeSeconds())
		.SetRealtimeUpdate(true)
	);

#pragma region ViewFamily配置
	//mViewFamily->SceneCaptureSource = SCS_FinalToneCurveHDR; // SCS_FinalColorLDR; //SCS_FinalToneCurveHDR;//
	InViewFamily->SceneCaptureSource = SCS_FinalColorLDR;

	InViewFamily->SetScreenPercentageInterface(new FCaptureScreenPercentageDriver(
			InViewFamily.ToSharedRef().Get(),
			this->mScenePercentage / 100.00f)
	);

	InViewFamily->ViewExtensions = GEngine->ViewExtensions->GatherActiveExtensions(FSceneViewExtensionContext(Scene));
	for (auto ViewExt : InViewFamily->ViewExtensions)
	{
		ViewExt->SetupViewFamily(*InViewFamily);
	}

	EngineShowFlagOverride(ESFIM_Game, InViewFamily->ViewMode, InViewFamily->EngineShowFlags, false);

	InViewFamily->bWorldIsPaused = this->bWorldPause;
	InViewFamily->ViewMode = ViewLightMode;
	InViewFamily->GammaCorrection = 2.2f;
	InViewFamily->EngineShowFlags = ShowFlags;
	//InViewFamily->EngineShowFlags.SetScreenPercentage(true);
	// Force enable view family show flag for HighDPI derived's screen percentage.
	InViewFamily->EngineShowFlags.ScreenPercentage = this->mScenePercentage;
	InViewFamily->EngineShowFlags.SetMotionBlur(false);
	InViewFamily->bIsHDR = false;
	InViewFamily->bRequireMultiView = false;

#pragma endregion viewfamily配置
	return InViewFamily;
}

FSceneView* UCaptureRenderTarget::SetupSceneView(TSharedPtr<FSceneViewFamilyContext> InViewFamily)
{
	FIntRect Rect(0, 0, GetRTT2D()->SizeX, GetRTT2D()->SizeY);
	EAntiAliasingMethod AAMethod = EAntiAliasingMethod::AAM_TemporalAA;

	FSceneViewInitOptions opt;
	opt.bUseFieldOfViewForLOD = true;
	opt.BackgroundColor = FColor::Transparent; //FColor::Black;
	opt.FOV = opt.DesiredFOV = this->mViewFOV;

	opt.SetViewRectangle(Rect);
	opt.ViewFamily = InViewFamily.Get();
	//计算默认视口位置
	{
		opt.ViewOrigin = this->mViewLocation;
		//refer FSceneView::UpdateViewMatrix
		FMatrix mat = FMatrix(FPlane(0, 0, 1, 0),
		                      FPlane(1, 0, 0, 0),
		                      FPlane(0, 1, 0, 0),
		                      FPlane(0, 0, 0, 1));
		opt.ViewRotationMatrix = FInverseRotationMatrix(this->mViewRotation) * mat;
		opt.ProjectionMatrix = this->mProjectionMatrix;
	}

	opt.SceneViewStateInterface = SceneViewState.GetReference();

	FSceneView* SceneView = new FSceneView(opt);
	InViewFamily->Views.Add(SceneView);

#pragma region SceneView配置
//#if RHI_RAYTRACING
//	SceneView->SetupRayTracedRendering();
//#endif
	SceneView->bCameraCut = false;
	SceneView->bIsOfflineRender = true;
	SceneView->bIsGameView = true;
	SceneView->bIsSceneCapture = false;
	SceneView->bIsMultiViewEnabled = false;
	SceneView->AntiAliasingMethod = AAMethod;
	SceneView->SetupAntiAliasingMethod();

	// process setting
	{
		SceneView->StartFinalPostprocessSettings(opt.ViewOrigin);
		{
			this->UpdateSceneViewPP(SceneView);

			SetupCustomPassMass(SceneView);
		}
		SceneView->EndFinalPostprocessSettings(opt);
	}

#pragma endregion SceneView配置
	return SceneView;
}

void UCaptureRenderTarget::UpdateSceneViewPP(FSceneView* SceneView)
{
	//SceneView->FinalPostProcessSettings.ScreenPercentage_DEPRECATED
	SceneView->FinalPostProcessSettings.ScreenPercentage_DEPRECATED = this->mScenePercentage;
	if (this->mPP)
	{
		const FPostProcessSettings& setting = this->mPP->Settings;
		//TODO:可改用TPropertyIterator反射迭代赋值

		SceneView->FinalPostProcessSettings.ColorGammaHighlights = setting.ColorGammaHighlights;
		SceneView->FinalPostProcessSettings.ColorGainMidtones = setting.ColorGainMidtones;
		SceneView->FinalPostProcessSettings.ColorGammaShadows = setting.ColorGammaShadows;
		SceneView->FinalPostProcessSettings.ColorGamma = setting.ColorGamma;

		SceneView->FinalPostProcessSettings.ColorGainHighlights = setting.ColorGainHighlights;
		SceneView->FinalPostProcessSettings.ColorGainMidtones = setting.ColorGainMidtones;
		SceneView->FinalPostProcessSettings.ColorGainShadows = setting.ColorGainShadows;
		SceneView->FinalPostProcessSettings.ColorGain = setting.ColorGain;


		SceneView->FinalPostProcessSettings.BloomMethod = setting.BloomMethod;
		SceneView->FinalPostProcessSettings.BloomIntensity = setting.BloomIntensity;
		SceneView->FinalPostProcessSettings.BloomThreshold = setting.BloomThreshold;

		SceneView->FinalPostProcessSettings.WhiteTemp = setting.WhiteTemp;
		SceneView->FinalPostProcessSettings.WhiteTint = setting.WhiteTint;
		SceneView->FinalPostProcessSettings.AutoExposureMethod = setting.AutoExposureMethod;
		SceneView->FinalPostProcessSettings.bOverride_AutoExposureApplyPhysicalCameraExposure = setting.bOverride_AutoExposureApplyPhysicalCameraExposure;
		SceneView->FinalPostProcessSettings.AutoExposureApplyPhysicalCameraExposure = setting.AutoExposureApplyPhysicalCameraExposure;
		SceneView->FinalPostProcessSettings.bOverride_AutoExposureBias = setting.bOverride_AutoExposureBias; // true;
		SceneView->FinalPostProcessSettings.AutoExposureBias = setting.AutoExposureBias; //1.0f;
		SceneView->FinalPostProcessSettings.bOverride_AutoExposureMinBrightness = setting.bOverride_AutoExposureMinBrightness; //true;
		SceneView->FinalPostProcessSettings.bOverride_AutoExposureMaxBrightness = setting.bOverride_AutoExposureMaxBrightness; //true;
		SceneView->FinalPostProcessSettings.AutoExposureMinBrightness = setting.AutoExposureMinBrightness; //1.0f;
		SceneView->FinalPostProcessSettings.AutoExposureMaxBrightness = setting.AutoExposureMaxBrightness; //1.0f;

		SceneView->FinalPostProcessSettings.bOverride_AutoExposureSpeedUp = setting.bOverride_AutoExposureSpeedUp; //true;
		SceneView->FinalPostProcessSettings.bOverride_AutoExposureSpeedDown = setting.bOverride_AutoExposureSpeedDown; //true;

		SceneView->FinalPostProcessSettings.AutoExposureSpeedUp = setting.AutoExposureSpeedUp; // 20.0f;
		SceneView->FinalPostProcessSettings.AutoExposureSpeedDown = setting.AutoExposureSpeedDown; // 20.0f;
		SceneView->FinalPostProcessSettings.WeightedBlendables = setting.WeightedBlendables;

#pragma region lumen
		SceneView->FinalPostProcessSettings.bOverride_LumenReflectionQuality = setting.bOverride_LumenReflectionQuality;
		SceneView->FinalPostProcessSettings.LumenReflectionQuality = setting.LumenReflectionQuality;
		SceneView->FinalPostProcessSettings.bOverride_LumenFinalGatherQuality = setting.bOverride_LumenFinalGatherQuality;
		SceneView->FinalPostProcessSettings.LumenFinalGatherQuality = FMath::Clamp(setting.LumenFinalGatherQuality, 2.0f, 100.0f);
#pragma endregion lumen
	}

}

void UCaptureRenderTarget::Capture(const uint8 PreCaptureFrameNum/* = 1*/)
{
	//check(GetState() == ECaptureState::Finished);
	this->PreRenderingFrameNum = PreCaptureFrameNum;
	this->SetState(ECaptureState::Renderering);
}

void UCaptureRenderTarget::CaptureFromLocationAndRotation(const FVector& Location, const FRotator& Rotator, const uint8 PreCaptureFrameNum)
{
	this->mViewLocation = Location;
	this->mViewRotation = Rotator;
	this->Capture(PreCaptureFrameNum);
}

bool UCaptureRenderTarget::GetRawData(UTextureRenderTarget2D* RTT, TArray64<uint8>& OutRawData, ECaptureCubeFace CubeFace)
{
	FRenderTarget* RenderTarget = RTT->GameThread_GetRenderTargetResource();
	EPixelFormat Format = PF_B8G8R8A8; //RTT->GetFormat();

	int32 ImageBytes = CalculateImageBytes(RTT->SizeX, RTT->SizeY, 0, Format);
	OutRawData.AddUninitialized(ImageBytes);
	bool bReadSuccess = false;

	{
		TArray<FColor> Colors;
		FReadSurfaceDataFlags RSDF(ERangeCompressionMode::RCM_UNorm, (ECubeFace)CubeFace);
		bReadSuccess = RenderTarget->ReadPixels(Colors);

		for (int i = 0; i < Colors.Num(); i++)
		{
			Colors[i].A = 255;
		}
		{
			/* 保留,FFloat16Color 转 FColor */
			//TArray<FFloat16Color> FloatColors;
			//bReadSuccess = RenderTarget->ReadFloat16Pixels(FloatColors);

			//for (int i = 0; i < FloatColors.Num(); i++)
			//{
			//	FColor TempColor;
			//	TempColor.R = FMath::Clamp<uint8>(FloatColors[i].R.GetFloat() * 255, 0, 255);
			//	TempColor.G = FMath::Clamp<uint8>(FloatColors[i].G.GetFloat() * 255, 0, 255);
			//	TempColor.B = FMath::Clamp<uint8>(FloatColors[i].B.GetFloat() * 255, 0, 255);
			//	TempColor.A = 255;
			//	Colors.Add(TempColor);
			//}
		}

		FMemory::Memcpy(OutRawData.GetData(), Colors.GetData(), ImageBytes);
	}

	if (bReadSuccess == false)
	{
		OutRawData.Empty();
	}

	return bReadSuccess;
}

bool UCaptureRenderTarget::IOToDisk(const TArray64<uint8>& rawData, const FString& SavePath, const FIntPoint& Size, EImageFormatEx ImageFormat, int Quility)
{
	bool bResult = false;

	{
		FArchive* Ar = IFileManager::Get().CreateFileWriter(*SavePath);
		if (Ar)
		{
			bool bSuceess = false;
			FBufferArchive Buffer;
			{
				int32 BitsPerPixel = 8;
				ERGBFormat RGBFormat = ERGBFormat::BGRA;
				if (ImageFormat == EImageFormatEx::WebP)
				{
					//按WebP格式走OpenCV进行编码
					cv::Mat tmp_mat(Size.Y, Size.X, CV_8UC4, const_cast<uint8*>(rawData.GetData()));

					static std::vector<uint8> CompressedData;

					std::vector<int> compress_params;
					compress_params.push_back(cv::IMWRITE_WEBP_QUALITY);
					compress_params.push_back(Quility);

					cv::imencode(".webp", tmp_mat, CompressedData, compress_params);

					Buffer.Serialize(CompressedData.data(), CompressedData.size());
					tmp_mat.empty();
					CompressedData.clear();
				}
				else
				{
					//png,jpg等
					static IImageWrapperModule& ImageWrapperModule = FModuleManager::Get().LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
					//强转换
					TSharedPtr<IImageWrapper> PNGImageWrapper = ImageWrapperModule.CreateImageWrapper((EImageFormat)((int8)(ImageFormat)-1));

					PNGImageWrapper->SetRaw(rawData.GetData(), rawData.GetAllocatedSize(), Size.X, Size.Y, RGBFormat, BitsPerPixel);

					const TArray64<uint8>& Data = PNGImageWrapper->GetCompressed(Quility);

					Buffer.Serialize((void*)Data.GetData(), Data.GetAllocatedSize());
				}

				bSuceess = true;
			}

			if (bSuceess)
			{
				Ar->Serialize(Buffer.GetData(), Buffer.Num());
				if (IsInGameThread())
				{
					this->OnSaveToDiskFinished().Broadcast(*SavePath);
					this->OnSaveToDiskFinished_BP.Broadcast(*SavePath);
				}
				else 
				{
					FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([this, SavePath]()
						{
							//从TaskGrap传递到gamethread进行事件分发
							this->OnSaveToDiskFinished().Broadcast(*SavePath);
							this->OnSaveToDiskFinished_BP.Broadcast(*SavePath);
						}, TStatId(), nullptr, ENamedThreads::GameThread);
				}
			}
			delete Ar;
		}
	}

	return bResult;
}

void UCaptureRenderTarget::SetupCustomPassMass(FSceneView* InSceneView)
{
	if (InSceneView)
	{
		if (!this->bNotInverseGamma)
		{
			{
				//Fixup:视乎这里被GC了..
				//Mat_PPGamma->AddToRoot();
				//UMaterialInstanceDynamic* MatInsDyn_PPGamma = InSceneView->State->GetReusableMID(Mat_PPGamma);
			}
			//将生命周期改变为当前UObject防止被Engine GC...导致RHI线程错误.
			if(MatInsDyn_PPGamma == nullptr || !MatInsDyn_PPGamma->IsValidLowLevel())
			{
				MatInsDyn_PPGamma = UMaterialInstanceDynamic::Create(Mat_PPGamma, this);
			}
			FPostProcessMaterialNode InitialNode(MatInsDyn_PPGamma, Mat_PPGamma->BlendableLocation, Mat_PPGamma->BlendablePriority, Mat_PPGamma->bIsBlendable);
			InSceneView->FinalPostProcessSettings.BlendableManager.PushBlendableData(1.0f, InitialNode);
		}
	}
}

void UCaptureRenderTarget::SaveToDisk(FString path, ECaptureCubeFace CubeFace, EImageFormatEx ImageFormat, int Quility)
{
	FString SavePath;
	if (path.IsEmpty())
	{
		FString Name = FString::Printf(TEXT("test_%s.png"), *FDateTime::Now().ToString());
		SavePath = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(), TEXT("textures"), TEXT("test"), *Name));
	}
	else
	{
		SavePath = path;
	}

	bool bSuccess = true;
	{
		check(this->GetRTT2D() != nullptr);
		FRenderTarget* RenderTarget = this->GetRTT2D()->GameThread_GetRenderTargetResource();

		FIntPoint Size = RenderTarget->GetSizeXY();

		TArray64<uint8> RawData;
		bSuccess = GetRawData(this->GetRTT2D(), RawData, CubeFace);

		this->IOToDisk(RawData, SavePath, Size, ImageFormat, Quility);
	}
}

void UCaptureRenderTarget::SaveToDiskAsync( FString path /*= TEXT("")*/, 
											ECaptureCubeFace CubeFace /*= ECaptureCubeFace::CubeFace_MAX*/, 
											EImageFormatEx ImageFormat /*= EImageFormatEx::PNG*/, 
											int Quility /*= 100*/)
{
	FString SavePath;
	if (path.IsEmpty())
	{
		FString Name = FString::Printf(TEXT("test_%s.png"), *FDateTime::Now().ToString());
		SavePath = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(), TEXT("textures"), TEXT("test"), *Name));
	}
	else
	{
		SavePath = path;
	}

	bool bSuceess = false;
	{
		check(this->GetRTT2D() != nullptr);
		FRenderTarget* RenderTarget = this->GetRTT2D()->GameThread_GetRenderTargetResource();

		FIntPoint Size = RenderTarget->GetSizeXY();

		TArray64<uint8> RawData;
		bSuceess = GetRawData(this->GetRTT2D(), RawData, CubeFace);
	
		//拷贝到logic线程...
		Async(EAsyncExecution::Thread, [this, SavePath, ImageFormat, Size, Quility, rawData = MoveTemp(RawData)]()
		{
			this->IOToDisk(rawData,SavePath, Size,ImageFormat, Quility);
		});
	}
}

ECaptureState UCaptureRenderTarget::GetState()
{
	return this->CaptureState;
}


void UCaptureRenderTarget::CalculateProjectionMatrix()
{
	// Calculate a Projection Matrix
	float XAxisFactor = 1.0f;
	float YAxisFactor = 1.0f;

	{
		//相机横纵比
		XAxisFactor = 1.0f;
		YAxisFactor = GetRTT2D()->SizeX / (float)GetRTT2D()->SizeY;
	}

	const float MinZ = GNearClippingPlane;
	const float MaxZ = MinZ;
	const float Radians_FOV = FMath::Max(0.00001f, this->mViewFOV) * (float)PI / 360.0f; //转弧度,并防止FOV为0度/0弧的情况

	//透视矩阵
	this->mProjectionMatrix = FReversedZPerspectiveMatrix(Radians_FOV,
	                                                      Radians_FOV,
	                                                      XAxisFactor,
	                                                      YAxisFactor,
	                                                      MinZ,
	                                                      MaxZ);
}

void UCaptureRenderTarget::FlushAsyncEngineSystems()
{
	// Flush Block until Level Streaming completes. This solves the problem where levels that are not controlled
	// by the Sequencer Level Visibility track are marked for Async Load by a gameplay system.
	// This will register any new actors/components that were spawned during this frame. This needs 
	// to be done before the shader compiler is flushed so that we compile shaders for any newly
	// spawned component materials.
	if (GetWorld())
	{
		GetWorld()->BlockTillLevelStreamingCompleted();
	}

#if WITH_EDITOR
	// Flush all assets handled by the asset compiling manager (i.e. textures, static meshes).
	// A progressbar is already in place so the user can get feedback while waiting for everything to settle.
	FAssetCompilingManager::Get().FinishAllCompilation();
#endif

	// Now we can flush the shader compiler. ToDo: This should probably happen right before SendAllEndOfFrameUpdates() is normally called
	if (GShaderCompilingManager)
	{
		bool bDidWork = false;
		int32 NumShadersToCompile = GShaderCompilingManager->GetNumRemainingJobs();
		if (NumShadersToCompile > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("[%d] Starting build for %d shaders."), GFrameCounter, NumShadersToCompile);
		}

		while (GShaderCompilingManager->GetNumRemainingJobs() > 0 || GShaderCompilingManager->HasShaderJobs())
		{
			UE_LOG(LogTemp, Log, TEXT("[%d] Waiting for %d shaders [Has Shader Jobs: %d] to finish compiling..."), GFrameCounter,
			       GShaderCompilingManager->GetNumRemainingJobs(), GShaderCompilingManager->HasShaderJobs());
			GShaderCompilingManager->ProcessAsyncResults(false, true);

			// Sleep for 1 second and then check again. This way we get an indication of progress as this works.
			FPlatformProcess::Sleep(1.f);
			bDidWork = true;
		}

		if (bDidWork)
		{
			UE_LOG(LogTemp, Log, TEXT("[%d] Done building %d shaders."), GFrameCounter, NumShadersToCompile);
		}
	}

	// Flush the Mesh Distance Field builder as well.
	if (GDistanceFieldAsyncQueue)
	{
		bool bDidWork = false;
		int32 NumDistanceFieldsToBuild = GDistanceFieldAsyncQueue->GetNumOutstandingTasks();
		if (NumDistanceFieldsToBuild > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("[%d] Starting build for %d mesh distance fields."), GFrameCounter, NumDistanceFieldsToBuild);
		}

		while (GDistanceFieldAsyncQueue->GetNumOutstandingTasks() > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("[%d] Waiting for %d Mesh Distance Fields to finish building..."), GFrameCounter,
			       GDistanceFieldAsyncQueue->GetNumOutstandingTasks());
			GDistanceFieldAsyncQueue->ProcessAsyncTasks();

			// Sleep for 1 second and then check again. This way we get an indication of progress as this works.
			FPlatformProcess::Sleep(1.f);
			bDidWork = true;
		}

		if (bDidWork)
		{
			UE_LOG(LogTemp, Log, TEXT("[%d] Done building %d Mesh Distance Fields."), GFrameCounter, NumDistanceFieldsToBuild);
		}
	}
	#if IS_ARTISTRENDERER
	// Flush the Mesh Distance Field builder as well.
	if (GDistanceFieldAsyncQueue2)
	{
		bool bDidWork = false;
		int32 NumDistanceFieldsToBuild = GDistanceFieldAsyncQueue2->GetNumOutstandingTasks();
		if (NumDistanceFieldsToBuild > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("cf [%d] Starting build for %d mesh distance fields."), GFrameCounter, NumDistanceFieldsToBuild);
		}

		while (GDistanceFieldAsyncQueue2->GetNumOutstandingTasks() > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("cf [%d] Waiting for %d Mesh Distance Fields to finish building..."), GFrameCounter,
				GDistanceFieldAsyncQueue2->GetNumOutstandingTasks());
			GDistanceFieldAsyncQueue2->ProcessAsyncTasks();

			// Sleep for 1 second and then check again. This way we get an indication of progress as this works.
			FPlatformProcess::Sleep(1.f);
			bDidWork = true;
		}

		if (bDidWork)
		{
			UE_LOG(LogTemp, Log, TEXT("cf [%d] Done building %d Mesh Distance Fields."), GFrameCounter, NumDistanceFieldsToBuild);
		}
	}
	#endif
	// if (GCardRepresentationAsyncQueue)
	// {
	// 	bool bDidWork = false;
	// 	int32 NumTasks = GCardRepresentationAsyncQueue->GetNumOutstandingTasks();
	// 	if (NumTasks > 0)
	// 	{
	// 		UE_LOG(LogTemp, Log, TEXT("cf [%d] Starting build for %d mesh cards."), GFrameCounter, NumTasks);
	// 	}
	//
	// 	while (GCardRepresentationAsyncQueue->GetNumOutstandingTasks() > 0)
	// 	{
	// 		UE_LOG(LogTemp, Log, TEXT("cf [%d] Waiting for %d Mesh Cards to finish building..."), GFrameCounter,
	// 		       GCardRepresentationAsyncQueue->GetNumOutstandingTasks());
	// 		GCardRepresentationAsyncQueue->ProcessAsyncTasks();
	//
	// 		// Sleep for 1 second and then check again. This way we get an indication of progress as this works.
	// 		FPlatformProcess::Sleep(1.f);
	// 		bDidWork = true;
	// 	}
	//
	// 	if (bDidWork)
	// 	{
	// 		//UE_LOG(LogMovieRenderPipeline, Log, TEXT("[%d] Done building %d Mesh Cards."), GFrameCounter, NumTasks);
	// 	}
	// }
#if IS_ARTISTRENDERER
	if (GCardRepresentationAsyncQueue2)
	{
		bool bDidWork = false;
		int32 NumTasks = GCardRepresentationAsyncQueue2->GetNumOutstandingTasks();
		if (NumTasks > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("cf [%d] Starting build for %d mesh cards."), GFrameCounter, NumTasks);
		}

		while (GCardRepresentationAsyncQueue2->GetNumOutstandingTasks() > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("cf [%d] Waiting for %d Mesh Cards to finish building..."), GFrameCounter,
				GCardRepresentationAsyncQueue2->GetNumOutstandingTasks());
			GCardRepresentationAsyncQueue2->ProcessAsyncTasks();

			// Sleep for 1 second and then check again. This way we get an indication of progress as this works.
			FPlatformProcess::Sleep(1.f);
			bDidWork = true;
		}

		if (bDidWork)
		{
			//UE_LOG(LogMovieRenderPipeline, Log, TEXT("[%d] Done building %d Mesh Cards."), GFrameCounter, NumTasks);
		}
	}
	#endif

	// Flush grass
	{
		for (TActorIterator<ALandscapeProxy> It(GetWorld()); It; ++It)
		{
			ALandscapeProxy* LandscapeProxy = (*It);
			if (LandscapeProxy)
			{
				TArray<FVector> CameraList;
				LandscapeProxy->UpdateGrass(CameraList, true);
			}
		}
	}

	// Flush virtual texture tile calculations
	ERHIFeatureLevel::Type FeatureLevel = GetWorld()->FeatureLevel;
	ENQUEUE_RENDER_COMMAND(VirtualTextureSystemFlushCommand)(
		[FeatureLevel](FRHICommandListImmediate& RHICmdList)
		{
			GetRendererModule().LoadPendingVirtualTextureTiles(RHICmdList, FeatureLevel);
		});
}

void UCaptureRenderTarget::RenderFrame()
{
#pragma region 通知RHI绘制到指定画布中

	//this->UpdateSceneView();
	//mCanvas->Clear(FLinearColor::Transparent);
	
	TSharedPtr<FCanvas> InCanvas = MakeShared<FCanvas>(FCanvas( this->mRTT2D->GameThread_GetRenderTargetResource(),
																nullptr,
																GetWorld(),
																ERHIFeatureLevel::SM5,
																FCanvas::CDM_DeferDrawing, //FCanvas::CDM_ImmediateDrawing, //
																1.0f));

	TSharedPtr<FSceneViewFamilyContext> InViewFamily = SetupViewFamily();
	SetupSceneView(InViewFamily);
	GetRendererModule().BeginRenderingViewFamily(InCanvas.Get(), InViewFamily.Get());
	PostRendererSubmission();
#pragma endregion 通知RHI绘制到指定画布中
	{
		//FlushRenderingCommands();
	}
}

void UCaptureRenderTarget::PostRendererSubmission()
{
	//Note:渲染管线队列保证会按序消费Command
	//FRenderTarget* RenderTarget = this->mCanvas->GetRenderTarget();
	ENQUEUE_RENDER_COMMAND(CaptureRenderFrameCommand)
	(
		[this](FRHICommandListImmediate& RHICmdList)
		{
			this->HandleFrame_OnRenderThread();
		}
	);

	//SaveToDisk();
}

void UCaptureRenderTarget::HandleFrame_OnRenderThread()
{
	check(IsInRenderingThread());

	this->CurPreFrame++;

	if (this->GetState() == ECaptureState::Warmup)
	{
		if (this->CurPreFrame >= CF_CaptureRenderTarget::DefaultWarmupFrameNum)
		{
			OnCaptureWarmupCompleted_RenderThread().Broadcast(this);

			this->SetState(ECaptureState::Renderering);
		}
	}
	else if (this->GetState() == ECaptureState::Renderering)
	{
		if (CurPreFrame >= this->PreRenderingFrameNum)
		{
			SetState(ECaptureState::Renderered);
		}
	}
	else if (this->GetState() == ECaptureState::Renderered)
	{
		SetState(ECaptureState::Captured);
		this->OnCaptureCompleted_RenderThread().Broadcast(this);
	}
	//do something
}

void UCaptureRenderTarget::Tick(float DeltaTime)
{
	switch (this->GetState())
	{
	case ECaptureState::Initilize:
		{
			break;
		}
	case ECaptureState::Warmup:
		{
			if (bAsync)
			{
				if (this->CurPreFrame <= CF_CaptureRenderTarget::DefaultWarmupFrameNum)
				{
					this->RenderFrame();
				}
			}
			else
			{
				if (!bStartWarmup)
				{
					bStartWarmup = true;
					for (int i = 0; i < CF_CaptureRenderTarget::DefaultWarmupFrameNum; i++)
					{
						this->RenderFrame();
					}
				}
			}
			break;
		}
	case ECaptureState::Renderering:
		{
			if (bAsync)
			{
				//XXX:maybe线程处理原因会触发多次问题,后面花时间处理一下
				//if(CurPreFrame == 0)
				//{	
				//	OnCaptureWarmupCompleted().Broadcast(this);
				//}

				if (this->CurPreFrame <= PreRenderingFrameNum)
				{
					this->RenderFrame();
				}
			}
			else
			{
				if (!bStartRender)
				{
					bStartRender = true;
					for (int i = 0; i < PreRenderingFrameNum; i++)
					{
						this->RenderFrame();
					}
				}
			}

			break;
		}
	case ECaptureState::Renderered:
		{
			this->RenderFrame();
			break;
		}
	case ECaptureState::Captured:
		{
			SetState(ECaptureState::Finished);
			this->OnCaptureCompleted().Broadcast(this);
			break;
		}
	case ECaptureState::Finished:
		{
			break;
		}
	default:
		{
			break;
		}
	}
	//this->Capture();
}

bool UCaptureRenderTarget::IsTickable() const
{
	if (GetDefault<UCaptureRenderTarget>() == this)
	{
		return false;
	}

	if (this->GetWorld()->IsPaused())
	{
		return false;
	}

	if (!GetRTT2D())
	{
		return false;
	}

	return true;
}
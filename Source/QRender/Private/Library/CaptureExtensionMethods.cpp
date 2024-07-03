// Fill out your copyright notice in the Description page of Project Settings.

#include "Library/CaptureExtensionMethods.h"

#include "Pipeline/CaptureRenderTarget.h"
#include "Kismet/GameplayStatics.h"

#include "Async/Async.h"
#include "Engine/PostProcessVolume.h"

#if WITH_OPENCV
#include "OpenCVHelper.h"
#include "IOpenCVHelperModule.h"
#include "PreOpenCVHeaders.h"
//OPENCV_INCLUDES_START
//#undef check // the check macro causes problems with opencv headers
#include "opencv2/opencv.hpp"
#include "opencv2/stitching.hpp"
#include "opencv2/imgproc.hpp"
//OPENCV_INCLUDES_END
#include "PostOpenCVHeaders.h"
#endif

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"

#include "Misc/FileHelper.h"
#include "Engine/TextureRenderTarget2D.h"

//#include "Media/VideoCapture.h"

#include "Camera/PlayerCameraManager.h"
#include "DTO/MedioDTOModel.h"

#include "ImageUtils.h"

void UCaptureExtensionMethods::ScreenShot_HighRes()
{
    FIntPoint size{1920, 1080};
    float factor = 2.0;
    UWorld *InWorld = GWorld;
    UCaptureRenderTarget *CaptureRTT = NewObject<UCaptureRenderTarget>(GWorld);
    CaptureRTT->AddToRoot();
    CaptureRTT->OnCaptureCompleted().AddLambda([&](UCaptureRenderTarget *c)
                                               {
		FString SavePath = TEXT("");
		c->SaveToDisk(SavePath);
		c->RemoveFromRoot();
		c = CaptureRTT = nullptr;

		Async(EAsyncExecution::Thread, [factor]()
		{
			// 延迟一下
			FPlatformProcess::Sleep(1.0f * factor);
			GEngine->ForceGarbageCollection();
		});
       });

    FVector location = UGameplayStatics::GetPlayerCameraManager(InWorld, 0)->GetCameraLocation();
    FRotator rot = UGameplayStatics::GetPlayerCameraManager(InWorld, 0)->GetCameraRotation();

    TArray<AActor *> arr_pp;
    UGameplayStatics::GetAllActorsOfClass(InWorld, APostProcessVolume::StaticClass(), arr_pp);
    APostProcessVolume *PP = Cast<APostProcessVolume>(arr_pp[0]); // arr_pp
    CaptureRTT->Initilize(size * factor, location, rot, 90.0f, 200.0f, true, true, PP);
}

void UCaptureExtensionMethods::HighScreenShot(UObject* WorldContextObject, EImageFormatEx InSaveType)
{
    // UE_LOG(LogTemp, Log, TEXT("-------------"));
    UScreenshotUserSettings* UserOpt = GetScreenshotUserSettings();
    FIntPoint size{ UserOpt->Width, UserOpt->Height };
    float factor = 2.0;

    UCaptureRenderTarget* CaptureRTT = NewObject<UCaptureRenderTarget>(WorldContextObject);
    CaptureRTT->AddToRoot();

#pragma region bind event

    CaptureRTT->OnCaptureCompleted().AddLambda([UserOpt](UCaptureRenderTarget* c)
    { 
        c->SaveToDiskAsync(UserOpt->SavePath, ECaptureCubeFace::CubeFace_MAX, UserOpt->ImageType, 100); 
    });

    CaptureRTT->OnSaveToDiskFinished().AddLambda([CaptureRTT, factor](const FString & SavePath)
                                                 {
		CaptureRTT->RemoveFromRoot();

		UCaptureExtensionMethods::GetScreenshotUserSettings()->OnHasScreenshotCompleted.Broadcast(SavePath);
		Async(EAsyncExecution::Thread, [factor, SavePath]()
		{
			//UCaptureExtensionMethods::OnHasScreenshotCompeleted().Broadcast(SavePath);
			// 延迟一下,在GC,否者会有线程冲突的情况发生
			FPlatformProcess::Sleep(1.0f * factor);
			GEngine->ForceGarbageCollection();
		}); 
    });

#pragma endregion bind event

    FVector location;
    FRotator rotation;
    APlayerCameraManager* CameraMgr = UGameplayStatics::GetPlayerCameraManager(WorldContextObject, 0);
    CameraMgr->GetCameraViewPoint(location, rotation);

    float fov = CameraMgr->GetFOVAngle();
    float screenPercent = 100.0f;
    {
        IConsoleVariable* Variable = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage"));
        screenPercent = Variable->GetFloat();
    }

    APostProcessVolume* PP = nullptr;
    {
        TArray<AActor*> arr_pp;
        UGameplayStatics::GetAllActorsOfClass(WorldContextObject, APostProcessVolume::StaticClass(), arr_pp);
        PP = Cast<APostProcessVolume>(arr_pp[0]); // arr_pp
    }

    CaptureRTT->Initilize(size, location, rotation, fov, screenPercent, false, true, PP);
}

void UCaptureExtensionMethods::CustomCameraHighScreenShot(
    UWorld* InWorld, EImageFormatEx InSaveType,
    const FVector& InLocation, const FRotator& InRotation, float InFov)
{
    // UE_LOG(LogTemp, Log, TEXT("-------------"));
    UScreenshotUserSettings* UserOpt = GetScreenshotUserSettings();
    FIntPoint size{ UserOpt->Width, UserOpt->Height };
    float factor = 2.0;

    UCaptureRenderTarget* CaptureRTT = NewObject<UCaptureRenderTarget>(InWorld);
    CaptureRTT->AddToRoot();

#pragma region bind event

    CaptureRTT->OnCaptureCompleted().AddLambda([UserOpt](UCaptureRenderTarget* c)
        {
            c->SaveToDiskAsync(UserOpt->SavePath, ECaptureCubeFace::CubeFace_MAX, UserOpt->ImageType, 100);
        });

    CaptureRTT->OnSaveToDiskFinished().AddLambda([CaptureRTT, factor](const FString & SavePath)
        {
          CaptureRTT->RemoveFromRoot();
            
            Async(EAsyncExecution::Thread, [factor, SavePath]()
                {
                    //UCaptureExtensionMethods::OnHasScreenshotCompeleted().Broadcast(SavePath);
                    // 延迟一下,在GC,否者会有线程冲突的情况发生
                    FPlatformProcess::Sleep(1.0f * factor);
                    GEngine->ForceGarbageCollection();
                    AsyncTask(ENamedThreads::GameThread ,[SavePath]()
                    {
                        UCaptureExtensionMethods::GetScreenshotUserSettings()->OnHasScreenshotCompleted.Broadcast(SavePath);
                    });
                });
        });

#pragma endregion bind event

    float screenPercent = 100.0f;
    {
        IConsoleVariable* Variable = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage"));
        screenPercent = Variable->GetFloat();
    }

    APostProcessVolume* PP = nullptr;
    {
        TArray<AActor*> arr_pp;
        UGameplayStatics::GetAllActorsOfClass(InWorld, APostProcessVolume::StaticClass(), arr_pp);
        PP = Cast<APostProcessVolume>(arr_pp[0]); // arr_pp
    }

    CaptureRTT->Initilize(size, InLocation, InRotation, InFov, screenPercent, false, true, PP);
}

void UCaptureExtensionMethods::CustomHighScreenShot(
    UWorld* InWorld, const FVector& InLocation, const FRotator& InRotation, float InFov,
    float InScreenPercent, const FVector2D& InSize,
    TFunction<void(const TArray64<uint8>&, const FVector2D&)> InCallback)
{
    UCaptureRenderTarget* CaptureRTT = NewObject<UCaptureRenderTarget>(InWorld);
    CaptureRTT->AddToRoot();
    float factor = 2.0;
    CaptureRTT->OnCaptureCompleted().AddLambda([CaptureRTT, factor, InSize, InCallback](UCaptureRenderTarget* c)
        {
            FRenderTarget* RenderTarget = c->GetRTT2D()->GameThread_GetRenderTargetResource();

            FIntPoint Size = RenderTarget->GetSizeXY();

            TArray64<uint8> RawData;
            bool bSuceess = UCaptureRenderTarget::GetRawData(c->GetRTT2D(), RawData, ECaptureCubeFace::CubeFace_MAX);

            InCallback(RawData, InSize);

            Async(EAsyncExecution::Thread, [factor]()
                {
                    // 延迟一下,在GC,否者会有线程冲突的情况发生
                    FPlatformProcess::Sleep(1.0f * factor);
                    GEngine->ForceGarbageCollection();
                });
            CaptureRTT->RemoveFromRoot();
        });
    APostProcessVolume* PP = nullptr;
    {
        TArray<AActor*> arr_pp;
        UGameplayStatics::GetAllActorsOfClass(InWorld, APostProcessVolume::StaticClass(), arr_pp);
        PP = Cast<APostProcessVolume>(arr_pp[0]); // arr_pp
    }
    FIntPoint size = FIntPoint(InSize.X, InSize.Y);
    CaptureRTT->Initilize(size, InLocation, InRotation, InFov, InScreenPercent, false, true, PP);
}

UMediaUserSettings *UCaptureExtensionMethods::GetMediaUserSettings()
{
    UMediaUserSettings *ret_obj = const_cast<UMediaUserSettings *>(GetDefault<UMediaUserSettings>());
    return ret_obj;
}

void UCaptureExtensionMethods::SaveMediaUserSettings()
{
    UMediaUserSettings *userOpt = GetMediaUserSettings();
    userOpt->SaveConfig();
}

UScreenshotUserSettings* UCaptureExtensionMethods::GetScreenshotUserSettings()
{
    UScreenshotUserSettings* userOpt = const_cast<UScreenshotUserSettings *>(GetDefault<UScreenshotUserSettings>());
    return userOpt;
}

void UCaptureExtensionMethods::SaveScreenshotUserSettings()
{
    UScreenshotUserSettings* userOpt = GetScreenshotUserSettings();
    userOpt->SaveConfig();
}

UTexture2D* UCaptureExtensionMethods::CaptureCameraFrame(UObject* WorldContextObject)
{
	FPerPlayerSplitscreenData& Data = GEngine->GameViewport->SplitscreenInfo[0].PlayerData[0];

    FViewport *InViewport = WorldContextObject->GetWorld()->GetGameViewport()->Viewport;
    FVector2D Size = InViewport->GetSizeXY();

    FIntPoint ShowViewportSizeMin;
    FIntPoint ShowViewportSizeMax;
    
    ShowViewportSizeMin.X = FMath::Lerp(0, Size.X, Data.OriginX);
    ShowViewportSizeMin.Y = FMath::Lerp(0, Size.Y, Data.OriginY);
    ShowViewportSizeMax.X = ShowViewportSizeMin.X + FMath::Lerp(0, Size.X, Data.SizeX);
    ShowViewportSizeMax.Y = ShowViewportSizeMin.Y + FMath::Lerp(0, Size.Y, Data.SizeY);

    //取最大且居中的正方形
    FIntRect InRect(ShowViewportSizeMin, ShowViewportSizeMax);
    FIntPoint center;
    FIntPoint extents;
    InRect.GetCenterAndExtents(center, extents);
    extents = FIntPoint(FMath::Min(extents.X, extents.Y), FMath::Min(extents.X, extents.Y));
	InRect = FIntRect(center - extents, center + extents);
    UTexture2D* NewTexture = UTexture2D::CreateTransient(InRect.Width(), InRect.Height(), PF_B8G8R8A8);
    NewTexture->UpdateResource();
    static FDelegateHandle delegate_once;
    auto fn_labmda = [NewTexture, InRect](FViewport *Viewport)
    {
        TArray<FColor> UncompressedData;
        if (Viewport && Viewport->ReadPixels(UncompressedData, FReadSurfaceDataFlags(), InRect))
        {
            for (FColor &color : UncompressedData)
            {
                color.A = 255;
            }

            if (NewTexture && NewTexture->IsValidLowLevelFast())
            {
                NewTexture->bNotOfflineProcessed = true;
                uint8 *MipData = static_cast<uint8 *>(NewTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

                // Bulk data was already allocated for the correct size when we called CreateTransient above
                FMemory::Memcpy(MipData, UncompressedData.GetData(), NewTexture->GetPlatformData()->Mips[0].BulkData.GetBulkDataSize());

                NewTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

                NewTexture->UpdateResource();
                // return NewTexture;
            }
        }
        UGameViewportClient::OnViewportRendered().Remove(delegate_once);
    };
    delegate_once = UGameViewportClient::OnViewportRendered().AddLambda(fn_labmda);

    return NewTexture;
}

void UCaptureExtensionMethods::CubeMap2Per()
{
    SIZE_T size;
    FPlatformProcess::GetApplicationMemoryUsage(FPlatformProcess::GetCurrentProcessId(), &size);
    UE_LOG(LogTemp, Log, TEXT(" GetApplicationMemoryUsage %d"), size);
}

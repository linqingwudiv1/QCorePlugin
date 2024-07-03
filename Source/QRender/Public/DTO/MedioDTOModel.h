// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MediaPacket.h"
#include "ImageFormatEx.h"
#include "MedioDTOModel.generated.h"

/**
 * 消费者的抽象
 */
class ICFVideoCaptureListener
{
public:
    virtual void OnMediaSample(const AVEncoder::FMediaPacket&Sample) = 0;
    //捕获完成时
    virtual void OnCaptureCompleted() = 0;

    virtual void OnStartCaptureMedia() = 0;

    //virtual bool StartImpl(double RingBufferDurationSecs = 30.0f, AActor *InTrackActor = nullptr) = 0;
    virtual bool StartImpl(FVideoCaptureOptions opt) = 0;
    //
    virtual void StopImpl() = 0;
};

USTRUCT(BlueprintType)
struct QRENDER_API FVideoTimelineKeys
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    float time;

    UPROPERTY(BlueprintReadWrite)
    FVector location;

    UPROPERTY(BlueprintReadWrite)
    FRotator rotation;
};

/**
 * 预留
 **/
USTRUCT(BlueprintType)
struct QRENDER_API FVideoCaptureOptions
{
    GENERATED_USTRUCT_BODY()
public:
    //内存帧缓存最大量(framerate * second)
    UPROPERTY(BlueprintReadWrite)
    float RingBufferDurationSecs = 30.0f;

    //如果keysInfo帧时间大于此取keys
    UPROPERTY(BlueprintReadWrite)
    float CaptureTimeSec = 30.0f;

    UPROPERTY(BlueprintReadWrite)
    TArray<FVideoTimelineKeys> keysInfo;

	UPROPERTY(BlueprintReadWrite)
	bool bTrackPawn = true;
};

UCLASS(BlueprintType, config = MediaCapture, defaultconfig)
class QRENDER_API UMediaUserSettings : public UObject 
{
    GENERATED_BODY()
public:
    //UPROPERTY(config)
    //bool bTrackPawn = false;

    //UPROPERTY(config)
    //TArray<FVideoTimelineKeys> keys;

    UPROPERTY(config)
    FString QualityType = TEXT("720P");

    UPROPERTY(config)
    FString SavePath;

    UPROPERTY(config)
    float BitRate = 5000000;

    UPROPERTY(config)
    int32 FrameRate = 30;

public:
    UFUNCTION(BlueprintCallable)
    FORCEINLINE FIntPoint GetSize()
    {
		//XXX:应该用枚举改善一下
        FIntPoint size = {};
		if (QualityType == TEXT("720P"))
		{
			size.X = 1280;
			size.Y = 720;
		}
		else if (QualityType == TEXT("1080P"))
		{
			size.X = 1920;
			size.Y = 1080;
		}
		else if (QualityType == TEXT("2K"))
		{
			size.X = 2880;
			size.Y = 1660;
		}
		else if (QualityType == TEXT("4K"))
		{
			size.X = 3840;
			size.Y = 2160;
		}
		return size;
    }


	UFUNCTION(BlueprintCallable)
	FORCEINLINE void UpdateBitRate()
	{
		if (QualityType == TEXT("720P"))
		{
			BitRate = FrameRate <= 30 ? 5000000 : 7500000;
		}
		else if (QualityType == TEXT("1080P"))
		{
			BitRate = FrameRate <= 30 ? 8000000 : 12000000;
		}
		else if (QualityType == TEXT("2K"))
		{
			BitRate = FrameRate <= 30 ? 16000000 : 24000000;
		}
		else if (QualityType == TEXT("4K"))
		{
			BitRate = FrameRate <= 30 ? 40000000 : 60000000;
		}
	}

    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetQualityType(const FString& _qualityType )
    {
        QualityType = _qualityType;
        this->UpdateBitRate();
    }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetFrameRate(const int32 _frameRate )
    {
        FrameRate = _frameRate;
        UpdateBitRate();
    }
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHasScreenshotCompleted, const FString&, SavePath);

UCLASS(BlueprintType, config = Screenshot, defaultconfig)
class QRENDER_API UScreenshotUserSettings : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(config)
    EImageFormatEx ImageType = EImageFormatEx::JPEG; //EScreenShotImageType::JPEG;
    
	UPROPERTY(config)
	FString QualityType = TEXT("1080P");

	UPROPERTY(config)
	FString SavePath;

    UPROPERTY(config)
    bool bAllowRenameSaveFile = false;

    UPROPERTY(config)
    bool bKeepRatio = false;

    UPROPERTY(config)
    int Width = 1280;

    UPROPERTY(config)
    int Height = 720;

    UPROPERTY(BlueprintAssignable)
    FOnHasScreenshotCompleted OnHasScreenshotCompleted;
public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FString GetImageType()
    {
        switch (this->ImageType)
        {
            case EImageFormatEx::JPEG:
            {
                return TEXT("JPG");
            }
		    case EImageFormatEx::PNG:
		    {
                return TEXT("PNG");
		    }
		    case EImageFormatEx::WebP:
		    {
                return TEXT("WEBP");
		    }
        }
        return TEXT("JPG");
    }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetImageType(const FString& InImageType)
    {
        if (InImageType == TEXT("JPG") || InImageType == TEXT("JPEG"))
        {
            this->ImageType = EImageFormatEx::JPEG;
        }
        else if (InImageType == TEXT("PNG") )
        {
            this->ImageType = EImageFormatEx::PNG;
        }
        else if (InImageType == TEXT("WEBP"))
        {
            this->ImageType = EImageFormatEx::WebP;
        }
    }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetQualityType(const FString& InQualityType)
    {
        this->QualityType = InQualityType;

        if(InQualityType == TEXT("1080P"))
        {
			this->Width = 1920;
			this->Height = 1080;
        }
        else if (InQualityType == TEXT("2K"))
        {
			this->Width = 2560;
			this->Height = 1440;
        }
        else if (InQualityType == TEXT("4K"))
        {
			this->Width = 3840;
			this->Height = 2160;
        }
        else if (InQualityType == TEXT("8K"))
        {
			this->Width = 7680;
			this->Height = 4320;
        }
        else if (InQualityType == TEXT("自定义"))
        {
            //由TS设置Width 和 Height
        }
    }
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IImageWrapper.h"

#include "Interfaces/IHttpRequest.h"
#include "ImageHelper.generated.h"


class UTexture2D;
class UTexture2DDynamic;
/**
 * 
 */
UCLASS()
class QCOREPLUGIN_API UImageHelper : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** */
	void LoadFormURL(const FString &Url, bool  bAutoGenerateMips = true, bool bForceGenerateMips = false);
	
	/** */
	void LoadFormURL(const FString &Url, UObject *target, void(UObject::* bindfunc)(bool, UTexture2D *), bool  bAutoGenerateMips = true, bool bForceGenerateMips = false);

	/** */
	void LoadFormURL(const FString &Url, void(bindfunc)(bool, UTexture2D *), bool  bAutoGenerateMips = true, bool bForceGenerateMips = false);

	static UTexture2D * LoadFromDisk(const FString &Path, bool  bAutoGenerateMips = true, bool bForceGenerateMips = false);


public:
	bool bLoading = false;
#pragma region Delegate Event

public:

	DECLARE_EVENT_TwoParams(FString, FOnLoadCompleted, bool,UTexture2D*);
	FOnLoadCompleted& OnLoadCompleted()
	{
		return Event_OnLoadCompleted;
	}
protected:
	FOnLoadCompleted Event_OnLoadCompleted;


public:
	DECLARE_EVENT_OneParam(FString, FOnLoadProgress, float)
	FOnLoadProgress& OnLoadProgress()
	{
		return Event_OnLoadProgress;
	}

protected:
	FOnLoadProgress Event_OnLoadProgress;

#pragma endregion

public:
	static UTexture2DDynamic* ConvertTexture2DToTexture2DDynamic(UTexture2D *target);
	static UTexture2D* ConvertTexture2DDynamicToTexture2D(UTexture2DDynamic *target);
};

/** 图片异步处理 */
UCLASS()
class QCOREPLUGIN_API UImageAsyncProcessor : public UObject
{
	GENERATED_BODY()


public:
	/** */
	static UImageAsyncProcessor * CreataFactory(bool  bAutoGenerateMips = true, bool bForceGenerateMips = false);
	/** */
	void Http_HandleCompleted(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bConnectedSuccessfully);

	void Http_Progress(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived);
#pragma region Delegate Event
public:

	DECLARE_EVENT_TwoParams(FString, FEvent_OnLoadCompleted, bool, UTexture2D*);
	FEvent_OnLoadCompleted& OnLoadCompleted()
	{
		return Event_OnLoadCompleted;
	}


	DECLARE_EVENT_OneParam(FString, FEvent_OnLoadProgress,  float);
	FEvent_OnLoadProgress& OnLoadProgress()
	{
		return Event_OnLoadProgress;
	}


protected:
	FEvent_OnLoadCompleted Event_OnLoadCompleted;
	FEvent_OnLoadProgress Event_OnLoadProgress;
#pragma endregion

private:
	bool bAutoGenerateMips = true;
	bool bForceGenerateMips = false; 
};
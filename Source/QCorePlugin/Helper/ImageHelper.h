// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IImageWrapper.h"

#include "Interfaces/IHttpRequest.h"
#include "ImageHelper.generated.h"

/**
 * 
 */
UCLASS()
class QCOREPLUGIN_API UImageHelper : public UObject
{
	GENERATED_BODY()

public:
	/** */
	void LoadFormURL(const FString &Url, bool  bAutoGenerateMips = true, bool bForceGenerateMips = false);
	
	/** */
	void LoadFormURL(const FString &Url, UObject *target, void(UObject::* bindfunc)(bool, class UTexture2D *), bool  bAutoGenerateMips = true, bool bForceGenerateMips = false);

	/** */
	void LoadFormURL(const FString &Url, void(bindfunc)(bool, UTexture2D *), bool  bAutoGenerateMips = true, bool bForceGenerateMips = false);

	static class UTexture2D * LoadFromDisk(const FString &Path, bool  bAutoGenerateMips = true, bool bForceGenerateMips = false);

#pragma region Delegate Event

public:

	DECLARE_EVENT_TwoParams(FString, FOnLoadCompleted, bool,class UTexture2D*);
	FOnLoadCompleted& OnLoadCompleted()
	{
		return Event_OnLoadCompleted;
	}

protected:
	FOnLoadCompleted Event_OnLoadCompleted;

#pragma endregion

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

#pragma region Delegate Event

public:

	DECLARE_EVENT_TwoParams(FString, FOnLoadCompleted, bool, class UTexture2D*);
	FOnLoadCompleted& OnLoadCompleted()
	{
		return Event_OnLoadCompleted;
	}
protected:
	FOnLoadCompleted Event_OnLoadCompleted;
#pragma endregion

private:
	bool bAutoGenerateMips = true;
	bool bForceGenerateMips = false; 
};
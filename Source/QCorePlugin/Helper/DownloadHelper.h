// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "DownloadHelper.generated.h"


USTRUCT()
struct FQHttpRanges
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
    int64 RangeStart ;
    
    UPROPERTY()
    int64 RangeEnd;

};


USTRUCT()
struct FQHttpProgress
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        /** 上传进度0.0-1.0 */
        float SentProgress;
    
    UPROPERTY()
		/** 下载进度0.0-1.0 */
        float ReceiveProgress;
    
    UPROPERTY()
        /** 新发送数据量 */
        int32 NewSent;

    UPROPERTY()
        /** 新接收数据量 */
        int32 NewReceive;

};

class URangeDownloadAsyncProcessor;
/**
 * 下载帮助器
 */
UCLASS(BlueprintType)
class QCOREPLUGIN_API UDownloadHelper : public UObject
{
	GENERATED_UCLASS_BODY()
public:
    friend URangeDownloadAsyncProcessor;
public:
    UFUNCTION(BlueprintCallable, Category = "QCore | DownloadHelper ")
        void StartByMemory(const FString &_Url);

    UFUNCTION(BlueprintCallable, Category = "QCore | DownloadHelper ")
        void StartByDisk( const FString& _Url, const FString &_SaveFilePath );
    
	UFUNCTION(BlueprintCallable, Category = "QCore | DownloadHelper ")
		void Start(const FString& _Url, bool bCacheFromDisk = false, const FString &_SaveFilePath = TEXT("") );

public:
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " QCore | DownloadHelper ")
			FString Url;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " QCore | DownloadHelper ")
			FString SaveFilePath;

            TArray64<uint8>& GetRawData();
protected:
    /**  */
    void Downloading();
    

    bool IsRequestable() const;

    void Clear() ;

    /**  */
    void ReqRange();

    /**  */
    void HandleOnRangeCompleted( URangeDownloadAsyncProcessor* Processor  ,
		                         bool bSuccessful,
		                         const FString&message,
		                         FHttpRequestPtr Request,
		                         FHttpResponsePtr Response
                                );

    void HandleOnRangeProgress(     
		URangeDownloadAsyncProcessor*  Processor,
		int32 BytesSent,
		int32 BytesReceived,
		int32 NewSent,
		int32 NewReceived
    );

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " QCore | DownloadHelper ")
        /**  */
        bool bIsCacheToMemory = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " QCore | DownloadHelper ")
		/**  */
        int MaxRequestNum = 15;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " QCore | DownloadHelper ")
        /**  */
        int MaxOnceRequestRange = 1024 * 256 ;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " QCore | DownloadHelper ")
		/** */
        int64 ContentLength = 0;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " QCore | DownloadHelper ")
        /** 已接收（下载）内容长度 */
        int64 ReceivedContentLength = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " QCore | DownloadHelper ")
        /** 已发送（上传）内容 */
        int64 SentContentLength = 0;

    /** */
    TArray<URangeDownloadAsyncProcessor*> HttpRangeProcessor;
    
    /**  */
    TArray<URangeDownloadAsyncProcessor*> HttpRangeErrorProcessor;

	/** */
	TArray64<uint8> rawData;

	/** 待请求分段 */
	TArray<FQHttpRanges> ReqStack;

#pragma region Delegate Event
public:
    DECLARE_EVENT_ThreeParams( FString, FEvent_OnDownloadCompleted,  UDownloadHelper* /** target*/      , 
                                                                     bool             /* bSuccessful */ , 
                                                                     const FString &  /* message */       )
    FEvent_OnDownloadCompleted& OnDownloadCompleted()
    {
        return Event_OnDownloadCompleted;
    }


protected:
    FEvent_OnDownloadCompleted Event_OnDownloadCompleted;

public:
	DECLARE_EVENT_TwoParams( FString, FEvent_OnDownloadProgress, UDownloadHelper* /* target           */, 
                                                                 const FQHttpProgress & /* data*/
                            )
    
    FEvent_OnDownloadProgress& OnDownloadProgress()
	{
		return Event_OnDownloadProgress;
	}

protected:
    FEvent_OnDownloadProgress Event_OnDownloadProgress;

#pragma endregion

};


UCLASS()
class QCOREPLUGIN_API URangeDownloadAsyncProcessor : public UObject
{
	GENERATED_BODY()
public:
    //friend UDownloadHelper;
    /** */
    static URangeDownloadAsyncProcessor* Create(UDownloadHelper* WorldCTX, const FString& Url, int64 from, int64 to);
    // static URangeDownloadAsyncProcessor
public:
    UFUNCTION()
    void Start();

protected:
    void HandleRequestCompleted( FHttpRequestPtr  Request, 
                                 FHttpResponsePtr Response,
                                 bool bConnectedSuccessfully );

    void HandleRequestProgress( FHttpRequestPtr Request, 
		                        int32 BytesSent,
		                        int32 BytesReceived         
                                );

#pragma region Delegate Event
public:
	DECLARE_EVENT_FiveParams(FString, FEvent_OnRangeCompleted, URangeDownloadAsyncProcessor*,
		                                                       bool /*bSuccessful*/         ,
		                                                       const FString& /*message*/   ,
		                                                       FHttpRequestPtr /*Request*/  ,
		                                                       FHttpResponsePtr /*Response*/
                            )
    FEvent_OnRangeCompleted& OnRangeCompleted()
	{
		return Event_OnRangeCompleted;
	}

protected:
    FEvent_OnRangeCompleted Event_OnRangeCompleted;

public:

    DECLARE_EVENT_FiveParams( FString, FEvent_OnRangeProgress, URangeDownloadAsyncProcessor* /** Processor */       ,
		                                                       int32                         /** BytesSent */       ,
		                                                       int32                         /** BytesReceived */   ,
                                                               int32                         /** NewSent */         ,
                                                               int32                         /** NewReceived */
                             )
    FEvent_OnRangeProgress& OnRangeProgress()
    {
        return Event_OnRangeProgress;
    }

protected:
    FEvent_OnRangeProgress Event_OnRangeProgress;
#pragma endregion Delegate Event
protected:
    int64 RangeStart;
    int64 RangeEnd;    
    UDownloadHelper* CTX;
    FString Url;
    int64 previousReceviced = 0;
    int64 previousSent = 0;
};

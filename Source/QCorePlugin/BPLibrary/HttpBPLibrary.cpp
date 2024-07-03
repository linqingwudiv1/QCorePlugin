// Fill out your copyright notice in the Description page of Project Settings.

#include "HttpBPLibrary.h"

#include "Helper/DownloadHelper.h"

UDownloadHelper* UHttpBPLibrary::DownloadRange(UObject* WorldContextObject, const FString& Url)
{
	UDownloadHelper* NewObj = NewObject<UDownloadHelper>(WorldContextObject, NAME_None, RF_Transient);
	
	NewObj->StartByDisk(Url, TEXT("d:/disk.png"));

	NewObj->OnDownloadCompleted().AddLambda([](UDownloadHelper* _DownloadHelper, bool bSuccessful , const FString &msg)
		{
			UE_LOG(LogTemp, Log, TEXT("Download Completed... %d %s"), bSuccessful ,*msg);
		});
	NewObj->OnDownloadProgress().AddLambda([](UDownloadHelper* _DownloadHelper,const FQHttpProgress& info )
		{
			UE_LOG(LogTemp, Log, TEXT("Download Progress... %d / %d   %f / %f"), info.NewReceive, info.NewSent, info.ReceiveProgress, info.SentProgress);
		});

	return NewObj;
}
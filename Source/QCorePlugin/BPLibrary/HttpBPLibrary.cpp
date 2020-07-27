// Fill out your copyright notice in the Description page of Project Settings.

#include "HttpBPLibrary.h"

#include "Helper/DownloadHelper.h"

UDownloadHelper* UHttpBPLibrary::DownloadRange(UObject* WorldContextObject, const FString& Url)
{
	UDownloadHelper* NewObj = NewObject<UDownloadHelper>(WorldContextObject, NAME_None, RF_Transient);
	
	NewObj->StartByDisk(Url, TEXT("d:/disk.png"));
	//NewObj->Start(Url);
	return NewObj;
}

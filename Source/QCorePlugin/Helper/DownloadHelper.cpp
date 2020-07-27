// Fill out your copyright notice in the Description page of Project Settings.


#include "DownloadHelper.h"

#include "BPLibrary/CoreBPLibrary.h"

#include "API/NetAPI.h"

#include "Misc/FileHelper.h"
#include "Internationalization/Regex.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "HAL/PlatformFilemanager.h"

UDownloadHelper::UDownloadHelper(const FObjectInitializer  & PCIP)
	: Super(PCIP)
{
}

void UDownloadHelper::StartByMemory(const FString& _Url)
{
	this->Start(_Url, false);
}

void UDownloadHelper::StartByDisk(const FString& _Url, const FString& _SaveFilePath)
{
	this->Start(_Url, true, _SaveFilePath);
}

void UDownloadHelper::Start(const FString& _Url, bool bCacheFromDisk, const FString& _SaveFilePath)
{
	this->Url = _Url;
	this->SaveFilePath = _SaveFilePath;
	this->bIsCacheToMemory = !bCacheFromDisk;
	
	this->Clear();
	
	TSharedRef<IHttpRequest> req = UNetAPI::createRequest(Url, TEXT("HEAD"));

	req->OnHeaderReceived().BindLambda([this]( FHttpRequestPtr Request,
											   const FString& HeaderName,
											   const FString& NewHeaderValue )
	{
		UE_LOG(LogTemp, Log, TEXT("OnHeaderReceived... %s  %s"), *HeaderName, *NewHeaderValue);
		if ( HeaderName == TEXT("Content-Length") )
		{
			ContentLength = FCString::Atoi(*NewHeaderValue);
		}
		int i = 0;
	});

	req->OnProcessRequestComplete().BindLambda([this](	FHttpRequestPtr Request	   ,
														FHttpResponsePtr Response  ,
														bool bConnectedSuccessfully  )
	{
		if ( bConnectedSuccessfully &&
			 Response->GetResponseCode() == 200 )
		{
			this->Downloading();
		}
	});

	req->ProcessRequest();
}

TArray64<uint8>& UDownloadHelper::GetRawData() 
{
	return this->rawData;
}

//uint8* p_temp; 
void UDownloadHelper::Downloading()
{
	if (this->bIsCacheToMemory)
	{
		rawData.Empty();
		rawData.SetNum(this->ContentLength);
	}
	else 
	{
		if ( FPaths::FileExists( this->SaveFilePath) )
		{
			
			FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*this->SaveFilePath);
		}

		uint8 temp_data = 0;

		FArchive* FileWriter = IFileManager::Get().CreateFileWriter(*this->SaveFilePath, EFileWrite::FILEWRITE_Append);

		FileWriter->Seek(this->ContentLength - 1L);
		//FileWriter->see
		FileWriter->Serialize(&temp_data, 1L);

		FileWriter->Close();
	}
	
	ReqStack.Empty();
	int32 MaxReq = ( ContentLength / MaxOnceRequestRange + ( ContentLength % MaxOnceRequestRange == 0 ? 0 : 1 ) );

	// Assign Ranges
	for (int i = 0; i < MaxReq; i++)
	{
		FQHttpRanges HttpRange;
		HttpRange.RangeStart = this->MaxOnceRequestRange * i;
		
		if ( i == MaxReq - 1 )
		{
			HttpRange.RangeEnd = this->MaxOnceRequestRange * i + ContentLength % this->MaxOnceRequestRange;
		}
		else 
		{
			HttpRange.RangeEnd = this->MaxOnceRequestRange * ( i + 1UL ) - 1UL;
		}

		this->ReqStack.Add(HttpRange);
	}

	while(IsRequestable())
	{
		this->ReqRange();
	}
}

bool UDownloadHelper::IsRequestable() const
{
	return ( this->MaxRequestNum > this->HttpRangeProcessor.Num() && this->ReqStack.Num() > 0 );
}

void UDownloadHelper::Clear()
{
	this->ContentLength			= 0L;
	this->ReceivedContentLength = 0L;
	this->SentContentLength		= 0L;

	this->ReqStack.Empty();
	this->HttpRangeProcessor.Empty();
	this->HttpRangeErrorProcessor.Empty();
}

void UDownloadHelper::ReqRange()
{
	FQHttpRanges&& HttpRange = ReqStack.Pop();
	URangeDownloadAsyncProcessor* processor = URangeDownloadAsyncProcessor::Create(this, Url, HttpRange.RangeStart, HttpRange.RangeEnd);

	this->HttpRangeProcessor.Add(processor);
	processor->OnRangeCompleted().AddUObject(this, &UDownloadHelper::HandleOnRangeCompleted);
	processor->OnRangeProgress().AddUObject(this, &UDownloadHelper::HandleOnRangeProgress);

	processor->Start();
}

void UDownloadHelper::HandleOnRangeCompleted(URangeDownloadAsyncProcessor* Processor, bool bSuccessful, const FString& message, FHttpRequestPtr Request, FHttpResponsePtr Response)
{
	this->HttpRangeProcessor.Remove(Processor);

	if (!bSuccessful)
	{
		this->HttpRangeErrorProcessor.Add(Processor);
	}

	if (this->ReqStack.Num() > 0)
	{
		ReqRange();
	}
	else
	{
		if ( this->HttpRangeProcessor.Num() == 0 && 
			 this->HttpRangeErrorProcessor.Num() <= 0 )
		{
			bool bHandleResult = this->HttpRangeErrorProcessor.Num() == 0;
			this->OnDownloadCompleted().Broadcast(this, bHandleResult, TEXT("Success") );
		}
		else 
		{
			UE_LOG(LogTemp, Log, TEXT("################################ Error reqeust num: %d"), this->HttpRangeErrorProcessor.Num());
		}
	}
}

void UDownloadHelper::HandleOnRangeProgress(URangeDownloadAsyncProcessor* Processor, int32 BytesSent, int32 BytesReceived, int32 NewSent, int32 NewReceived)
{
	ReceivedContentLength	+= NewReceived;
	SentContentLength		+= NewSent;

	FQHttpProgress Progress;
	Progress.NewSent			= NewSent;
	Progress.NewReceive			= NewReceived;
	Progress.ReceiveProgress	= ReceivedContentLength  / (float)this->ContentLength;
	Progress.SentProgress		= SentContentLength		 / (float)this->ContentLength;

	UE_LOG(LogTemp, Log, TEXT("RangeProgress: ReceiveProgress:{%f}. new Receive: {%d} | SentProgress:{%f}, New Sent:{%d} "), Progress.ReceiveProgress, 
																															 Progress.NewReceive,
																															 Progress.SentProgress,
																															 Progress.NewSent);

	this->OnDownloadProgress().Broadcast(this, Progress);
}

URangeDownloadAsyncProcessor* URangeDownloadAsyncProcessor::Create(UDownloadHelper* WorldCTX, const FString &Url, int64 from, int64 to)
{
	URangeDownloadAsyncProcessor* ret_obj = NewObject<URangeDownloadAsyncProcessor>(WorldCTX, NAME_None, RF_Transient);

	ret_obj->CTX = WorldCTX;
	ret_obj->Url = Url;
	ret_obj->RangeStart = from;
	ret_obj->RangeEnd = to;

	return ret_obj;
}

void URangeDownloadAsyncProcessor::Start()
{
	TSharedRef<IHttpRequest> req = UNetAPI::createRequestRange(Url,this->CTX->rawData.Num(), this->RangeStart, this->RangeEnd);
	
	req->OnProcessRequestComplete().BindUObject(this, &URangeDownloadAsyncProcessor::HandleRequestCompleted);
	req->OnRequestProgress().BindUObject(this, &URangeDownloadAsyncProcessor::HandleRequestProgress);

	if ( req->ProcessRequest() )
	{
	}
}

void URangeDownloadAsyncProcessor::HandleRequestCompleted(	FHttpRequestPtr Request,
															FHttpResponsePtr Response, 
															bool bConnectedSuccessfully )
{
	if ( bConnectedSuccessfully &&
		 Response->GetResponseCode() == 206 )
	{
		TArray<uint8> NewRawData = Response->GetContent();

		if (CTX->bIsCacheToMemory)
		{
			// update memory data
			uint8* pData = CTX->rawData.GetData();
			FMemory::Memcpy( (pData + this->RangeStart), NewRawData.GetData(), NewRawData.Num());
		}
		else
		{
			FArchive* FileWriter = nullptr;
			int i = 0;
			do
			{
				//防止 IO 堵塞
				if (i > 0)
				{

					FPlatformProcess::Sleep(0.01f);
				}
				FileWriter = IFileManager::Get().CreateFileWriter(*this->CTX->SaveFilePath, EFileWrite::FILEWRITE_Append | EFileOpenFlags::IO_WRITE);
				i++;
			}
			while (FileWriter == nullptr);

			FileWriter->Seek(this->RangeStart);
			FileWriter->Serialize(NewRawData.GetData(), NewRawData.Num());
			FileWriter->Close();
		}

		this->OnRangeCompleted().Broadcast(this, true, TEXT("Success"), Request, Response);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("#####################################---------- error..."));
		//this->OnRangeCompleted().Broadcast(this, false, TEXT("Error"), Request, Response);
	}
}


void URangeDownloadAsyncProcessor::HandleRequestProgress( FHttpRequestPtr Request, 
														  int32 BytesSent, 
														  int32 BytesReceived )
{
	int64 NewReceived		= BytesReceived - previousReceviced;
	int32 NewSent			= BytesSent - previousSent;
	this->previousSent = previousSent;
	this->previousReceviced = BytesReceived;

	this->OnRangeProgress().Broadcast(this, BytesSent, BytesReceived, NewSent, NewReceived);
}

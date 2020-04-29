// Fill out your copyright notice in the Description page of Project Settings.


#include "ImageHelper.h"

#include "BPLibrary/CoreBPLibrary.h"

#include "API/NetAPI.h"
#include "Modules/ModuleManager.h"
#include "IImageWrapperModule.h"
#include "Engine/Texture2D.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Engine/Public/ImageUtils.h"

//求平均
#define AVERAGE(a, b)   (uint8)( ((a) + (b)) >> 1 )

#define TEXTURE2D_MIN_SIZE  256
#define TEXTURE2D_MAX_SIZE  2048

#pragma region Graphs Algorithm

//双立方插值
static void InterpolationScale(const uint8 * SrcData, int SrcWidth, int SrcHeight, uint8 * DstData, int DstWidth, int DstHeight)
{

	float w_scaleFactor = float(SrcWidth)  / DstWidth;
	float h_scaleFactor = float(SrcHeight) / DstHeight;


	TArray<int32> arr_w;
	TArray<int32> arr_h;

	arr_w.SetNum(DstWidth);
	arr_h.SetNum(DstHeight);

	for (int ih = 0; ih < arr_h.Num(); ih++)
	{
		arr_h[ih] = ih * h_scaleFactor;
	}
	for (int iw = 0; iw < arr_w.Num(); iw++)
	{
		arr_w[iw] = iw * w_scaleFactor;
	}

	const uint8* pSrcPtr = SrcData;
	uint8* pDestPtr = DstData;

	const int DstW_Pixel = DstWidth << 2;
	for (int ih = 0; ih < DstHeight; ih++)
	{
		pSrcPtr = SrcData +  ( ( arr_h[ih] * SrcWidth ) << 2 );
		pDestPtr = DstData + ( ( ih * DstWidth ) << 2 );

		for (int iw = 0; iw < DstW_Pixel; iw += 4)
		{
			//arr_w[ ( iw >> 2)] * 4
			int temp_pixelc = arr_w[(iw >> 2)] << 2;
			
			*( pDestPtr + 0 + iw ) = *( pSrcPtr + 0 + temp_pixelc );
			*( pDestPtr + 1 + iw ) = *( pSrcPtr + 1 + temp_pixelc );
			*( pDestPtr + 2 + iw ) = *( pSrcPtr + 2 + temp_pixelc );
			*( pDestPtr + 3 + iw ) = *( pSrcPtr + 3 + temp_pixelc );
		}
	}
}

//二分均值
static void MinifyByTwoScale(const uint8 * SrcData, uint8 * dstData, int srcWidth, int srcHeight)
{
	int x, y, x2, y2;
	int TgtWidth, TgtHeight;
	TgtWidth = srcWidth / 2;
	TgtHeight = srcHeight / 2;
	for (y = 0; y < TgtHeight; y++)
	{
		y2 = (y << 1);
		for (x = 0; x < TgtWidth; x++)
		{
			x2 = (x << 1);

			int p_Temp	   =	( y2 * srcWidth + x2	 ) << 2;
			int p_TempNext =	( y2 * srcWidth + x2 + 1 ) << 2;

			int q_Temp		=	( (y2 + 1)	* srcWidth + x2 )	 << 2;
			int q_TempNext	=	( (y2 + 1)	* srcWidth + x2 + 1) << 2;

			int dst_Temp = (y*TgtWidth + x) << 2;

			uint8 pb = AVERAGE(SrcData[ p_Temp + 0 ], SrcData[ p_TempNext + 0 ]);
			uint8 pg = AVERAGE(SrcData[ p_Temp + 1 ], SrcData[ p_TempNext + 1 ]);
			uint8 pr = AVERAGE(SrcData[ p_Temp + 2 ], SrcData[ p_TempNext + 2 ]);
			uint8 pa = AVERAGE(SrcData[ p_Temp + 3 ], SrcData[ p_TempNext + 3 ]);
			
			uint8 qb = AVERAGE(SrcData[ q_Temp + 0 ], SrcData[ q_TempNext + 0 ]);
			uint8 qg = AVERAGE(SrcData[ q_Temp + 1 ], SrcData[ q_TempNext + 1 ]);
			uint8 qr = AVERAGE(SrcData[ q_Temp + 2 ], SrcData[ q_TempNext + 2 ]);
			uint8 qa = AVERAGE(SrcData[ q_Temp + 3 ], SrcData[ q_TempNext + 3 ]);
			
			
			dstData[dst_Temp + 0] = AVERAGE(pb, qb);
			dstData[dst_Temp + 1] = AVERAGE(pg, qg);
			dstData[dst_Temp + 2] = AVERAGE(pr, qr);
			dstData[dst_Temp + 3] = AVERAGE(pa, qa);
		}
	}
}

/**
 * 纹理的像素级处理逻辑
 * @param	imgW 图片的宽
 * @param	imgH 图片的高
 * @param	rawData 像素Raw数据, 格式:PF_B8G8R8A8
 * @param	bAutoGenerateMips  是否生成MipMap.	 Note:必须确保图片长宽都是2的N次方或为正方形图片
 * @param	bForceGenerateMips 是否强行生成Mips. Note:会导致非2的N次方的非方形图片拉伸变形
 */
UTexture2D* Handle_T2D(int imgW, int imgH, const TArray<uint8>* rawData, bool  bAutoGenerateMips = true, bool bForceGenerateMips = false)
{

	UTexture2D* LoadedT2D = nullptr;
	uint8* TextureData = nullptr;

	uint32 ScaleW = FMath::Clamp<uint32>(FMath::RoundUpToPowerOfTwo(imgW), TEXTURE2D_MIN_SIZE, TEXTURE2D_MAX_SIZE);
	uint32 ScaleH = FMath::Clamp<uint32>(FMath::RoundUpToPowerOfTwo(imgH), TEXTURE2D_MIN_SIZE, TEXTURE2D_MAX_SIZE);

	bool bGenerateMips = false;
	bool bResizeSize = false;

	if ((imgW == imgH) ||
		(ScaleW == imgW && ScaleH == imgH))
	{
		bGenerateMips = bAutoGenerateMips;
	}
	else
	{
		bGenerateMips = bForceGenerateMips;
	}

	if (ScaleW != imgW ||
		ScaleH != imgH)
	{
		bResizeSize = true;
	}
	else
	{
		bResizeSize = false;
	}

#pragma region 创建T2D

	if (bResizeSize)
	{
		LoadedT2D = UTexture2D::CreateTransient(ScaleW, ScaleH, PF_B8G8R8A8);
		TextureData = static_cast<uint8*>(LoadedT2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
		InterpolationScale(rawData->GetData(), imgW, imgH, TextureData, ScaleW, ScaleH);
	}
	else
	{
		LoadedT2D = UTexture2D::CreateTransient(imgW, imgH, PF_B8G8R8A8);
		TextureData = static_cast<uint8*>(LoadedT2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
		FMemory::Memcpy(TextureData, rawData->GetData(), rawData->Num());
	}

	LoadedT2D->SRGB = true;

	LoadedT2D->PlatformData->Mips[0].BulkData.Unlock();
#pragma endregion


#pragma region 更新T2D


	uint32 curW = FMath::Max<uint32>(ScaleW >> 1, 1);
	uint32 curH = FMath::Max<uint32>(ScaleH >> 1, 1);


#pragma region 生成Mipmaps
	if (bGenerateMips)
	{
		const uint8 *pre_data = TextureData;
		int pow_W = (int)FMath::Log2(ScaleW);
		int pow_H = (int)FMath::Log2(ScaleH);
		int powNum = FMath::Max(pow_W, pow_H);
		for (int i = 0; i < powNum; i++)
		{
			int32 NumBytes = (curW * curH) * 4;
			FTexture2DMipMap* Mip = new FTexture2DMipMap();
			LoadedT2D->PlatformData->Mips.Add(Mip);

			Mip->SizeX = curW;
			Mip->SizeY = curH;

			Mip->BulkData.Lock(LOCK_READ_WRITE);
			Mip->BulkData.Realloc(NumBytes);
			Mip->BulkData.Unlock();

			uint8* Data = static_cast<uint8*>(Mip->BulkData.Lock(LOCK_READ_WRITE));
			int newWidth  = ( i < pow_W ? (curW * 2) : 1);
			int newHeight = ( i < pow_H ? (curH * 2) : 1);

			MinifyByTwoScale(pre_data,
				Data, 
				newWidth, 
				newHeight
			);

			pre_data = Data;

			Mip->BulkData.Unlock();

			curW = FMath::Max<uint32>(curW >> 1, 1);
			curH = FMath::Max<uint32>(curH >> 1, 1);
		}
	}
#pragma endregion

	LoadedT2D->UpdateResource();

	return LoadedT2D;
}



EImageFormat GetImageFormatFromPath(const FString &path)
{
	EImageFormat ImageFormat = EImageFormat::JPEG;
	{
		TArray<FString> arr_regex;

		bool isJPG = UCoreBPLibrary::RegexMatch(path, TEXT(".\\.jpg$|.\\.jpeg$|.\\.jpe$"), arr_regex);
		arr_regex.Empty();
		bool isPNG = UCoreBPLibrary::RegexMatch(path, TEXT(".\\.png$"), arr_regex);
		arr_regex.Empty();
		bool isHDR = UCoreBPLibrary::RegexMatch(path, TEXT(".\\.exr$|.\\.hdr$"), arr_regex);
		arr_regex.Empty();

		if (isPNG)
		{
			ImageFormat = EImageFormat::PNG;
		}

		if (isJPG)
		{
			ImageFormat = EImageFormat::JPEG;
		}

		if (isHDR)
		{
			ImageFormat = EImageFormat::EXR;
		}
	}

	return ImageFormat;
}



#pragma endregion

void UImageHelper::LoadFormURL( const FString &Url	   , 
									    bool bAutoGenerateMips , 
									    bool bForceGenerateMips )
{
	if (Url.IsEmpty())
	{
		UE_LOG(LogHttp, Error, TEXT("----------------------- Url is Empty..."));
		return;
	}

	FString encodeUrl = UCoreBPLibrary::UrlEncode(Url);

	//中文转码
	auto req = UNetAPI::createRequst(encodeUrl, TEXT("GET"), TEXT("application/octet-stream;image/png;image/jpeg;"));
	
	UImageAsyncProcessor* netProcessor = UImageAsyncProcessor::CreataFactory(bAutoGenerateMips, bForceGenerateMips);
	netProcessor->OnLoadCompleted().AddLambda([=](bool bSuccessful, UTexture2D* t2d)
	{
		this->OnLoadCompleted().Broadcast(bSuccessful, t2d);
	});

	req->OnProcessRequestComplete().BindUObject(netProcessor, &UImageAsyncProcessor::Http_HandleCompleted);

	if (!req->ProcessRequest())
	{
		UE_LOG(LogHttp, Error, TEXT("----------------------- request Process Request invaild"));
	}
}

void UImageHelper::LoadFormURL( const FString & Url, 
								UObject *target , 
								void(UObject::* bindfunc)(bool, UTexture2D *), 
								bool bAutoGenerateMips,
								bool bForceGenerateMips)
{
	this->OnLoadCompleted().AddUObject(target, bindfunc);
	this->LoadFormURL(Url);
}

void UImageHelper::LoadFormURL( const FString & Url , 
								void(bindfunc)(bool, UTexture2D *) , 
								bool bAutoGenerateMips,
								bool bForceGenerateMips )
{
	//this->OnLoadCompleted().AddUObject(target, bindfunc);
	this->OnLoadCompleted().AddStatic(bindfunc);

	this->LoadFormURL(Url);
}


UTexture2D * UImageHelper::LoadFromDisk(const FString & Path,
												bool bAutoGenerateMips, 
												bool bForceGenerateMips )
{

	TArray<uint8> binarayData;
	if (!FFileHelper::LoadFileToArray(binarayData, *Path))
	{
		return NULL;
	}

	UTexture2D* LoadedT2D = NULL;
	const TArray<uint8>* rawData = nullptr;
	EImageFormat ImageFormat = GetImageFormatFromPath(Path);

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);

	if (!ImageWrapper.IsValid() ||
		!ImageWrapper->SetCompressed(binarayData.GetData(), binarayData.Num()))
	{
		UE_LOG(LogHttp, Error, TEXT("can't Set Compressed or ImageWrapper is InValid"));
		return nullptr;
	}

	if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, rawData))
	{
		UE_LOG(LogTemp, Warning, TEXT("can't get Raw Data"));
		return nullptr;
	}

	int32 w = ImageWrapper->GetWidth();
	int32 h = ImageWrapper->GetHeight();

	LoadedT2D = Handle_T2D(w, h, rawData, bAutoGenerateMips, bForceGenerateMips);
	return LoadedT2D;
}

UImageAsyncProcessor * UImageAsyncProcessor::CreataFactory( bool  bAutoGenerateMips, bool bForceGenerateMips)
{
	UImageAsyncProcessor *Processor = NewObject<UImageAsyncProcessor>();

	Processor->bAutoGenerateMips  = bAutoGenerateMips;
	Processor->bForceGenerateMips = bForceGenerateMips;
	
	return Processor;
}

void UImageAsyncProcessor::Http_HandleCompleted(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bConnectedSuccessfully)
{
	FString Url = Req->GetURL();
	if ( !this->IsValidLowLevel() || 
		 !bConnectedSuccessfully ||
		 !Res.IsValid() ||
		 Res->GetResponseCode() != 200 )
	{
		UE_LOG(LogHttp, Error, TEXT("-------- : req occur error from Url {%s} : error info: {%s}."), *Url, *(Res->GetContentAsString()));
		this->OnLoadCompleted().Broadcast(false, nullptr);

		return;
	}

#pragma region 从Url判断图片类型/NOte:也可以从Content-Type中判断

	EImageFormat ImageFormat = GetImageFormatFromPath(Url);

#pragma endregion
	TArray<uint8> binarayData = Res->GetContent();
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);

	if (!ImageWrapper.IsValid() ||
		!ImageWrapper->SetCompressed(binarayData.GetData(), binarayData.Num()))
	{
		UE_LOG(LogHttp, Error, TEXT("can't Set Compressed or ImageWrapper is InValid"));
		this->OnLoadCompleted().Broadcast(false, nullptr);
		return;
	}

	const TArray<uint8>* rawData = nullptr;

	if ( !ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, rawData) )
	{
		UE_LOG(LogTemp, Warning, TEXT("can't get Raw Data"));
		this->OnLoadCompleted().Broadcast(false, nullptr);
		return;
	}

	int32 w = ImageWrapper->GetWidth();
	int32 h = ImageWrapper->GetHeight();

	auto t2d = Handle_T2D( w, h, rawData, this->bAutoGenerateMips, this->bForceGenerateMips );

	this->OnLoadCompleted().Broadcast(true, t2d);
}

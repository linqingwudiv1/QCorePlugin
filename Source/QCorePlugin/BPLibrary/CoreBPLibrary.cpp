// Fill out your copyright notice in the Description page of Project Settings.

#include "CoreBPLibrary.h"


#include "Engine/Engine.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Engine/Texture2D.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetInputLibrary.h"
#include "Regex.h"


#include "Framework/Application/SlateApplication.h"
#include "Widgets/SViewport.h"

//
#include "IDesktopPlatform.h"
#include "QDesktopPlatformModule.h"

#include "Helper/ImageHelper.h"

#pragma region Static Method

static bool IsAllowedChar(UTF8CHAR LookupChar)
{
	static char AllowedChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-+=_.~:/#@?&!";
	static bool bTableFilled = false;
	static bool AllowedTable[256] = { false };

	if (!bTableFilled)
	{
		for (int32 Idx = 0; Idx < UE_ARRAY_COUNT(AllowedChars) - 1; ++Idx)	// -1 to avoid trailing 0
		{
			uint8 AllowedCharIdx = static_cast<uint8>(AllowedChars[Idx]);

			check(AllowedCharIdx < UE_ARRAY_COUNT(AllowedTable));
			AllowedTable[AllowedCharIdx] = true;
		}

		bTableFilled = true;
	}

	return AllowedTable[LookupChar];
}

#pragma endregion


UCoreBPLibrary::UCoreBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


bool UCoreBPLibrary::RegexMatch(const FString& Str, const FString& Pattern, TArray<FString>& Result)
{
	FRegexPattern MatherPatter(Pattern);
	FRegexMatcher Matcher(MatherPatter, Str);

	while (Matcher.FindNext())
	{
		Result.Add(Matcher.GetCaptureGroup(0));
	}

	return Result.Num() == 0 ? false : true;
}



TArray<FString> UCoreBPLibrary::OpenFileDialog( UObject * WorldContextObject,
												const FString &Title  ,
												const FString defPath ,
												const FString& FileTypes ,
												EM_EFileDialogFlags Flags )
{
	void* ParentWindowPtr = FSlateApplication::Get().GetActiveTopLevelWindow()->GetNativeWindow()->GetOSWindowHandle();
	TArray<FString> arr_outfile;
	IDesktopPlatform* deskPlatform = nullptr;
	
	deskPlatform = FQDesktopPlatformModule::Get();
	
	
	FString def_path = defPath.IsEmpty() ? FPaths::ProjectDir() : defPath;
	
	bool isSuccess = deskPlatform->OpenFileDialog(ParentWindowPtr,
		*Title,
		def_path,
		TEXT("") ,
		FileTypes /*TEXT("打开图片|*.png;*.jpg;*.jpeg")*/,
		(uint32)Flags,
		arr_outfile);
	
	return arr_outfile;
}

UTexture2D * UCoreBPLibrary::LoadTexture2DFromDisk(const FString &Path, bool  bAutoGenerateMips, bool bForceGenerateMips)
{
	return UImageHelper::LoadFromDisk(Path, bAutoGenerateMips, bForceGenerateMips);
}

int32 UCoreBPLibrary::GetMeshMaterialNum(UStaticMesh * mesh)
{
	int32 num = mesh->StaticMaterials.Num();
	
	return num;
}

bool UCoreBPLibrary::executeShellCMD(const FString & cmd)
{
#if PLATFORM_WINDOWS
	const char* convert_cmd = TCHAR_TO_UTF8(*cmd);
	system(convert_cmd);
	return true;
#else
	UE_LOG(LogTemp, Error, TEXT("platform is not support"));
	return false;
#endif
}

#include <iostream>
#include "string.h"
#define Buffer_Size 30
FString UCoreBPLibrary::ReadFromIPC(UObject * WorldContextObject)
{
	FString ret_result = TEXT("");
	
	uint8 arr_data[Buffer_Size] = { 0 };
	
	UCoreBPLibrary::GetNamedPipe()->ReadBytes( Buffer_Size, arr_data );
	
	const std::string cstr(reinterpret_cast<const char*>(arr_data), Buffer_Size );
	
	UE_LOG(LogTemp, Log, TEXT("-----------------------中文linqing字符串测试@#$%Test"));
	
	ret_result = FString(cstr.c_str());
	
	return ret_result;
}

bool UCoreBPLibrary::WriteFromIPC(UObject * WorldContextObject)
{
	char* str = "abc123456";
	
	bool result = UCoreBPLibrary::GetNamedPipe()->WriteBytes(10, str);
	
	while(true)
	{
		if (!UCoreBPLibrary::bIsRW(WorldContextObject)) 
		{
			continue;
		}
	
		uint8 buffer[128];
		bool read_result = UCoreBPLibrary::GetNamedPipe()->ReadBytes( 128 * sizeof(uint8) , buffer);
		
		std::string cstr(reinterpret_cast<const char*>(buffer), 128 * sizeof(uint8));
		FString temp_str = FString(cstr.c_str());
	
		UE_LOG(LogTemp, Log, TEXT("-----------------------  %s \r\n  ,	\
								   ----------------------- %s"), buffer, *temp_str)
		break;
	}
	
	return result;
}

bool UCoreBPLibrary::bIsRW(UObject * WorldContextObject)
{
	return UCoreBPLibrary::GetNamedPipe()->IsReadyForRW();
}

FPlatformNamedPipe* UCoreBPLibrary::GetNamedPipe(const FString& PipeName, bool bServer, bool bAsync)
{
	static FPlatformNamedPipe p;
	const int BufferSize = 128;
	//static bCreate
	if (!p.IsCreated())
	{
		bool isSuccess = p.Create(PipeName, bServer, bAsync);
		
		if (!isSuccess) 
		{
			return nullptr;
		}
	}

	return &p;
}


FString UCoreBPLibrary::UrlEncode( const FString &UnencodedString)
{
	FTCHARToUTF8 Converter(*UnencodedString);	//url encoding must be encoded over each utf-8 byte
	const UTF8CHAR* UTF8Data = (UTF8CHAR*)Converter.Get();	//converter uses ANSI instead of UTF8CHAR - not sure why - but other code seems to just do this cast. In this case it really doesn't matter
	FString EncodedString = TEXT("");

	TCHAR Buffer[2] = { 0, 0 };

	for (int32 ByteIdx = 0, Length = Converter.Length(); ByteIdx < Length; ++ByteIdx)
	{
		UTF8CHAR ByteToEncode = UTF8Data[ByteIdx];

		if (IsAllowedChar(ByteToEncode))
		{
			Buffer[0] = ByteToEncode;
			FString TmpString = Buffer;
			EncodedString += TmpString;
		}
		else if (ByteToEncode != '\0')
		{
			EncodedString += TEXT("%");
			EncodedString += FString::Printf(TEXT("%.2X"), ByteToEncode);
		}
	}
	return EncodedString;
}

void UCoreBPLibrary::AdjustViewportSize(UObject * WorldContextObject, FMargin padding)
{
	if (WorldContextObject->GetWorld() && GEngine)
	{

		UGameViewportClient* gameViewport = GEngine->GameViewport;
		//WorldObjectContext->GetWorld()->App
		FVector2D ViewportSize;
		
		gameViewport->GetViewportSize(ViewportSize);
		
		// The GameViewport takes some time to initialize
		if (ViewportSize.X > 0 && ViewportSize.Y > 0)
		{
			FMargin relativeMat = FMargin
			{
				padding.Left	    / ViewportSize.X ,
				padding.Top		    / ViewportSize.Y ,
				1 - padding.Right   / ViewportSize.X ,
				1 - padding.Bottom  / ViewportSize.Y
			};

			gameViewport->SplitscreenInfo[0].PlayerData[0].OriginX = relativeMat.Left;
			gameViewport->SplitscreenInfo[0].PlayerData[0].OriginY = relativeMat.Top;

			gameViewport->SplitscreenInfo[0].PlayerData[0].SizeX   = relativeMat.Right;
			gameViewport->SplitscreenInfo[0].PlayerData[0].SizeY   = relativeMat.Bottom;
		}
	}
}

FString UCoreBPLibrary::GetSoftObjectPtrRefPath(TSoftObjectPtr<class USkeletalMesh> ptr)
{
	FSoftObjectPath objPath = ptr.ToSoftObjectPath();
	return objPath.GetAssetPathString();
}

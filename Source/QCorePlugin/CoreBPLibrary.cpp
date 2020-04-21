// Fill out your copyright notice in the Description page of Project Settings.

#include "CoreBPLibrary.h"


#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Engine/Texture2D.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetInputLibrary.h"
#include "Regex.h"


#pragma region Static Method


static bool IsAllowedChar(UTF8CHAR LookupChar)
{
	static char AllowedChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-+=_.~:/#@?&!";
	static bool bTableFilled = false;
	static bool AllowedTable[256] = { false };

	if (!bTableFilled)
	{
		for (int32 Idx = 0; Idx < ARRAY_COUNT(AllowedChars) - 1; ++Idx)	// -1 to avoid trailing 0
		{
			uint8 AllowedCharIdx = static_cast<uint8>(AllowedChars[Idx]);
			check(AllowedCharIdx < ARRAY_COUNT(AllowedTable));
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
	UE_LOG(LogTemp, Log, TEXT("platform is not support"));
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

int UCoreBPLibrary::CreateIPC(UObject * WorldContextObject)
{
	return 0;

//#if PLATFORM_WINDOWS
//	#define BUFSIZE 512
//	// Open the named pipe
//	// Most of these parameters aren't very relevant for pipes.
//	HANDLE pipe = CreateFile(
//		L"\\\\.\\pipe\\my_pipe",
//		GENERIC_READ, // only need read access
//		FILE_SHARE_READ | FILE_SHARE_WRITE,
//		NULL,
//		OPEN_EXISTING,
//		FILE_ATTRIBUTE_NORMAL,
//		NULL
//	);
//
//	if (pipe == INVALID_HANDLE_VALUE) 
//	{
//		UE_LOG(LogTemp, Log, TEXT("Failed to connect to pipe."));
//		return 1;
//	}
//	
//	UE_LOG(LogTemp, Log, TEXT("Reading data from pipe..."));
//	wchar_t buffer[128];
//	DWORD numBytesRead = 0;
//
//	BOOL result = ReadFile(
//		pipe ,
//		buffer , // the data from the pipe will be put here
//		127 * sizeof(wchar_t) , // number of bytes allocated
//		&numBytesRead , // this will store number of bytes actually read
//		NULL // not using overlapped IO
//	);
//
//	if (result) 
//	{
//		buffer[numBytesRead / sizeof(wchar_t)] = '\0'; // null terminate the string
//		
//		UE_LOG(LogTemp, Log, TEXT("Number of bytes read: %d"), numBytesRead );
//		UE_LOG(LogTemp, Log, TEXT("Number of bytes read: %s"), buffer);
//	}
//	else 
//	{
//		UE_LOG(LogTemp, Log, TEXT("Failed to read data from the pipe."));
//	}
//	// Close our pipe handle
//	CloseHandle(pipe);
//	return 0;
//#endif

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

TArray<FString> UCoreBPLibrary::OpenFileDialog(UObject * WorldContextObject)
{
	IDesktopPlatform* deskPlatform = FDesktopPlatformModule::Get();
	TArray<FString> arr_outfile;
	FString def_path = FPaths::ProjectDir();

	bool isSuccess = deskPlatform->OpenFileDialog(nullptr ,
												  TEXT("打开图片") ,
												  def_path ,
												  TEXT("") ,
												  TEXT("打开图片|*.png;*.jpg;*.jpeg") ,
												  EFileDialogFlags::None ,
												  arr_outfile );

	return arr_outfile;
}
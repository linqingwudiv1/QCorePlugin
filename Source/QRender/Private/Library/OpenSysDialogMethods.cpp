// Fill out your copyright notice in the Description page of Project Settings.

#include "Library/OpenSysDialogMethods.h"


#include "GenericPlatform/GenericPlatformMisc.h"
#include "GenericPlatform/GenericPlatformProcess.h"

#include "Engine.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/COMPointer.h"
#include <shlobj.h>
#include "Windows/HideWindowsPlatformTypes.h"

bool UOpenSysDialogMethods::OpenSystemFileDialog(FString& OutFilePath, const FString& Title, const FString& DefaultPath, ESystemFileDialogTypeEx Type, const FString& Filter)
{
	OutFilePath.Empty();
	TComPtr<IFileDialog> FileDialog;

	CLSID type_clsid;
	IID type_iid;

	switch( Type)
	{
		case ESystemFileDialogTypeEx::Open: { type_clsid = CLSID_FileOpenDialog; type_iid = IID_IFileOpenDialog; break; }
		case ESystemFileDialogTypeEx::Save: { type_clsid = CLSID_FileSaveDialog; type_iid = IID_IFileSaveDialog; break; }
	}

	//auto c = CLSID_FileSaveDialog;
	if (!SUCCEEDED(::CoCreateInstance( type_clsid,
									   nullptr,
									   CLSCTX_INPROC_SERVER,
									   type_iid,
									   IID_PPV_ARGS_Helper(&FileDialog)
									  )))
	{
		return false;
	}
	// Set up common settings
	FileDialog->SetTitle( *Title);

	FString DefaultWindowsPath = DefaultPath;
	if (DefaultWindowsPath.IsEmpty() )
	{
		DefaultWindowsPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	}

	DefaultWindowsPath.ReplaceInline(TEXT("/"), TEXT("\\"), ESearchCase::CaseSensitive);
	TComPtr<IShellItem> DefaultPathItem;
	if (SUCCEEDED(::SHCreateItemFromParsingName(*DefaultWindowsPath, nullptr, IID_PPV_ARGS(&DefaultPathItem))))
	{
		FileDialog->SetFolder(DefaultPathItem);
	}
	TArray<FString> UnformattedExtensions;
	TArray<COMDLG_FILTERSPEC> FileDialogFilters;
	{
		Filter.ParseIntoArray(UnformattedExtensions, TEXT("|"), true);
		if (UnformattedExtensions.Num() % 2 == 0)
		{
			FileDialogFilters.Reserve(UnformattedExtensions.Num() / 2);
			for (int32 ExtensionIndex = 0; ExtensionIndex < UnformattedExtensions.Num();)
			{
				COMDLG_FILTERSPEC& NewFilterSpec = FileDialogFilters[FileDialogFilters.AddDefaulted()];
				NewFilterSpec.pszName = *UnformattedExtensions[ExtensionIndex++];
				NewFilterSpec.pszSpec = *UnformattedExtensions[ExtensionIndex++];
			}
		}
	}
	FileDialog->SetFileTypes(FileDialogFilters.Num(), FileDialogFilters.GetData());
	// show
	if (!SUCCEEDED(
		FileDialog->Show(static_cast<HWND>(FSlateApplication::Get().GetActiveTopLevelWindow()->GetNativeWindow()->
			GetOSWindowHandle()))))
	{
		return false;
	}
	TComPtr<IShellItem> Result;
	if (!SUCCEEDED(FileDialog->GetResult(&Result))) { return false; }
	PWSTR pFilePath = nullptr;
	if (SUCCEEDED(Result->GetDisplayName(SIGDN_FILESYSPATH, &pFilePath)))
	{
		FString SaveFilePath = pFilePath;
		int32 FilterIndex = 0;
		if (SUCCEEDED(FileDialog->GetFileTypeIndex(reinterpret_cast<UINT*>(&FilterIndex))))
		{
			FilterIndex -= 1; // GetFileTypeIndex returns a 1-based index
		}
		if (FileDialogFilters.IsValidIndex(FilterIndex))
		{
			const FString ExtensionPattern = FileDialogFilters[FilterIndex].pszSpec;
			TArray<FString> SaveExtensions;
			ExtensionPattern.ParseIntoArray(SaveExtensions, TEXT(";"));
			FString CleanExtension = SaveExtensions[0];
			if (CleanExtension == TEXT("*.*"))
			{
				CleanExtension.Reset();
			}
			else
			{
				int32 WildCardIndex = INDEX_NONE;
				if (CleanExtension.FindChar(TEXT('*'), WildCardIndex))
				{
					CleanExtension = CleanExtension.RightChop(WildCardIndex + 1);
				}
			}
			FString SaveFileName = FPaths::GetCleanFilename(SaveFilePath);
			SaveFilePath = FPaths::GetPath(SaveFilePath);
			if (FPaths::GetExtension(SaveFileName).IsEmpty() && !CleanExtension.IsEmpty())
			{
				SaveFileName = FPaths::SetExtension(SaveFileName, CleanExtension);
			}
			OutFilePath = FPaths::ConvertRelativePathToFull(FPaths::Combine(SaveFilePath, SaveFileName));
		}
	}
	::CoTaskMemFree(pFilePath);

	return true;
}

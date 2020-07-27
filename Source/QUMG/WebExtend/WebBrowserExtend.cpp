// Copyright Epic Games, Inc. All Rights Reserved.

#include "WebBrowserExtend.h"
#include "SWebBrowserExtend.h"

#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Async/TaskGraphInterfaces.h"
#include "UObject/ConstructorHelpers.h"

#if WITH_EDITOR
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialExpressionMaterialFunctionCall.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialFunction.h"
#include "Factories/MaterialFactoryNew.h"
#include "AssetRegistryModule.h"
#include "PackageHelperFunctions.h"
#endif

#define LOCTEXT_NAMESPACE "WebBrowserExtend"

/////////////////////////////////////////////////////
// UWebBrowserExtend

UWebBrowserExtend::UWebBrowserExtend(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;
}

void UWebBrowserExtend::LoadURL(const FString& NewURL)
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->LoadURL(NewURL);
	}
}

void UWebBrowserExtend::SetInitialURL(const FString& NewURL)
{
	this->InitialURL = NewURL;
}

void UWebBrowserExtend::LoadString(const FString& Contents, const FString& DummyURL)
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->LoadString(Contents, DummyURL);
	}
}

void UWebBrowserExtend::ExecuteJavascript(const FString& ScriptText)
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->ExecuteJavascript(ScriptText);
	}
}

FText UWebBrowserExtend::GetTitleText() const
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->GetTitleText();
	}

	return FText::GetEmpty();
}

FString UWebBrowserExtend::GetUrl() const
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->GetUrl();
	}

	return FString();
}

void UWebBrowserExtend::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	WebBrowserWidget.Reset();
}

void UWebBrowserExtend::HandleLoadCompleted()
{
#if UE_EDITOR

#else
	//禁用右键上下文菜单
	FString script = TEXT("window.oncontextmenu = function(event) { event.preventDefault(); event.stopPropagation(); return false; };");
	this->ExecuteJavascript(script);
#endif
	OnLoadCompleted().Broadcast();
}

TSharedRef<SWidget> UWebBrowserExtend::RebuildWidget()
{
	if (IsDesignTime())
	{
		return SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Web Browser extend", "Web Browser extend"))
			];
	}
	else
	{
		WebBrowserWidget = SNew(SWebBrowserExtend)
			.InitialURL(InitialURL)
			#if UE_EDITOR
			.ShowControls(true)
			#else
			.ShowControls(false)
			#endif
			.EnableMouseTransparency(this->bMouseTransparency)
			.EnableVirtualPointerTransparency(this->bVirtualPointerTransparency)
			.TransparencyDelay(this->TransparencyDelay)
			.TransparencyThreshold(this->TransparencyThreshold)
			.BrowserFrameRate(this->BrowserFrameRate)
			.SupportsTransparency(bSupportsTransparency)
			.OnUrlChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged))
			.OnBeforePopup(BIND_UOBJECT_DELEGATE(FOnBeforePopupDelegate, HandleOnBeforePopup))
			.OnLoadCompleted(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleLoadCompleted));

		//第三方输入法支持
		if (WebBrowserWidget.IsValid())
		{
			ITextInputMethodSystem* const TextInputMethodSys = FSlateApplication::Get().GetTextInputMethodSystem();
			WebBrowserWidget->BindInputMethodSystem(TextInputMethodSys);
		}

		return WebBrowserWidget.ToSharedRef();
	}
}

void UWebBrowserExtend::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (WebBrowserWidget.IsValid())
	{

	}
}

void UWebBrowserExtend::HandleOnUrlChanged(const FText& InText)
{
	OnUrlChanged.Broadcast(InText);
}

bool UWebBrowserExtend::HandleOnBeforePopup(FString URL, FString Frame)
{
	if (OnBeforePopup.IsBound())
	{
		if (IsInGameThread())
		{
			OnBeforePopup.Broadcast(URL, Frame);
		}
		else
		{
			// Retry on the GameThread.
			TWeakObjectPtr<UWebBrowserExtend> WeakThis = this;
			FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, URL, Frame]()
				{
					if (WeakThis.IsValid())
					{
						WeakThis->HandleOnBeforePopup(URL, Frame);
					}
				}, TStatId(), nullptr, ENamedThreads::GameThread);
		}

		return true;
	}

	return false;
}

void UWebBrowserExtend::BindUObject(FString Name, UObject* Object)
{
	if (this && WebBrowserWidget.IsValid())
	{
		WebBrowserWidget->BindUObject(Name, Object);
	}
}

void UWebBrowserExtend::DispatchEvent(const FString& eventName, const FString& paramjson)
{
	FString jsScript = FString::Printf(TEXT("document.dispatchEvent(new CustomEvent('%s', {detail: %s})); "), *eventName, *paramjson);

	UE_LOG(LogTemp, Log, TEXT("----------------------  js script :%s "), *jsScript);

	this->ExecuteJavascript(jsScript);
}

#if WITH_EDITOR

const FText UWebBrowserExtend::GetPaletteCategory()
{
	return LOCTEXT("QUI", "QUI");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

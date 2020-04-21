// Fill out your copyright notice in the Description page of Project Settings.


#include "WebBrowserEx.h"

#include "WebBrowser.h"
#include "SWebBrowser.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Async/TaskGraphInterfaces.h"
#include "UObject/ConstructorHelpers.h"


#include "Runtime/Slate/Public/Framework/Application/SlateApplication.h"

#define LOCTEXT_NAMESPACE "WebBrowserEx"



UWebBrowserEx::UWebBrowserEx(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UWebBrowserEx::BindUObject( FString Name, UObject* Object)
{
	if ( this && WebBrowserWidget.IsValid() ) 
	{
		WebBrowserWidget->BindUObject(Name, Object);
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("-------------------------- WebBrowserWidget is invalid.") );
	}
}

void UWebBrowserEx::Init(const FString & Url, bool bTransparency)
{
	this->InitialURL = Url; 
	this->bSupportsTransparency = bTransparency;
}

void UWebBrowserEx::DispatchEvent(const FString & eventName, const FString & paramjson)
{
	FString jsScript = FString::Printf(TEXT("document.dispatchEvent(new CustomEvent('%s', {detail: %s})); "), *eventName, *paramjson);
	UE_LOG(LogTemp, Log, TEXT("----------------------  js script :%s "),*jsScript);

	this->ExecuteJavascript(jsScript);

}

void UWebBrowserEx::SynchronizeProperties()
{
	UWebBrowser::SynchronizeProperties();
}

void UWebBrowserEx::ReleaseSlateResources(bool bReleaseChildren)
{
	UWebBrowser::ReleaseSlateResources(bReleaseChildren);
}
#if WITH_EDITOR
const FText UWebBrowserEx::GetPaletteCategory()
{
	return LOCTEXT("QingEx", "QingEx");
}
#endif

TSharedRef<SWidget> UWebBrowserEx::RebuildWidget()
{
	if (IsDesignTime())
	{
		return SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Web BrowserEx", "Web BrowserEx"))
			];
	}


	WebBrowserWidget = SNew(SWebBrowser)
		.InitialURL(InitialURL)
		.ShowControls(false)
		.SupportsTransparency(bSupportsTransparency)
		.BackgroundColor(FColor{ 255, 255, 255, 0 })
		.OnUrlChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged))
		.OnBeforePopup(BIND_UOBJECT_DELEGATE(FOnBeforePopupDelegate, HandleOnBeforePopup))
		.OnLoadCompleted(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleLoadCompleted));

	//中文输入法支持
	if ( WebBrowserWidget.IsValid() )
	{
		ITextInputMethodSystem* const TextInputMethodSys = FSlateApplication::Get().GetTextInputMethodSystem();
		WebBrowserWidget->BindInputMethodSystem(TextInputMethodSys);
	}

	return WebBrowserWidget.ToSharedRef();
}

void UWebBrowserEx::HandleLoadCompleted()
{	
	//禁用右键上下文菜单
	FString script = TEXT("window.oncontextmenu = function(event) { event.preventDefault(); event.stopPropagation(); return false; };");
	this->ExecuteJavascript(script);
	OnLoadCompleted().Broadcast();
}

#undef LOCTEXT_NAMESPACE
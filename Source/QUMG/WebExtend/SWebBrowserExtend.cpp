// Copyright Epic Games, Inc. All Rights Reserved.

#include "SWebBrowserExtend.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SThrobber.h"
#include "Widgets/Layout/SBorder.h"

#pragma region Extend
#include "SWebBrowser.h"
#include "WebBrowserModule.h"
#include "IWebBrowserPopupFeatures.h"
#include "IWebBrowserSingleton.h"
#include "IWebBrowserWindow.h"
#include "Framework/Application/SlateApplication.h"

#include "Input/Events.h"
#include "Input/Reply.h"
#include "RHI.h"
#include "RenderUtils.h"
#include "RenderingThread.h"
#pragma endregion Extend


#define LOCTEXT_NAMESPACE "WebBrowser Extend"

SWebBrowserExtend::SWebBrowserExtend()
{
	bMouseTransparency = false;
	bVirtualPointerTransparency = false;

	TransparencyDelay = 0.0f;
	TransparencyThreadshold = 0.333f;

	LastMousePixel = FLinearColor::White;
	LastMouseTime = 0.0f;
}

SWebBrowserExtend::~SWebBrowserExtend()
{
}

void SWebBrowserExtend::Construct(const FArguments& InArgs)
{
	OnLoadCompleted = InArgs._OnLoadCompleted;
	OnLoadError = InArgs._OnLoadError;
	OnLoadStarted = InArgs._OnLoadStarted;
	OnTitleChanged = InArgs._OnTitleChanged;
	OnUrlChanged = InArgs._OnUrlChanged;
	OnBeforeNavigation = InArgs._OnBeforeNavigation;
	OnLoadUrl = InArgs._OnLoadUrl;
	OnShowDialog = InArgs._OnShowDialog;
	OnDismissAllDialogs = InArgs._OnDismissAllDialogs;
	OnBeforePopup = InArgs._OnBeforePopup;
	OnCreateWindow = InArgs._OnCreateWindow;
	OnCloseWindow = InArgs._OnCloseWindow;

	OnUnhandledKeyDown = InArgs._OnUnhandledKeyDown;
	OnUnhandledKeyUp = InArgs._OnUnhandledKeyUp;
	bShowInitialThrobber = InArgs._ShowInitialThrobber;

	#pragma region Extend

	bMouseTransparency = InArgs._EnableMouseTransparency;
	bVirtualPointerTransparency = InArgs._EnableVirtualPointerTransparency;

	TransparencyDelay = FMath::Max(0.0f, InArgs._TransparencyDelay);
	TransparencyThreadshold = FMath::Clamp(InArgs._TransparencyThreshold, 0.0f, 1.0f);

	FCreateBrowserWindowSettings Settings;

	Settings.BrowserFrameRate = FMath::Clamp(InArgs._BrowserFrameRate, 1, 120);
	Settings.bUseTransparency = true;
	Settings.BackgroundColor = InArgs._BackgroundColor;
	Settings.InitialURL = InArgs._InitialURL;
	Settings.ContentsToLoad = InArgs._ContentsToLoad;
	Settings.bShowErrorMessage = UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG;
	Settings.bThumbMouseButtonNavigation = false;
	
	IWebBrowserSingleton* Singleton = IWebBrowserModule::Get().GetSingleton();

	if (Singleton)
	{
		Singleton->SetDevToolsShortcutEnabled(Settings.bShowErrorMessage);
		BrowserWindow = Singleton->CreateBrowserWindow(Settings);
	}

	#pragma endregion Extend

	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SAssignNew(BrowserView, SWebBrowserView, BrowserWindow)
				.ParentWindow(InArgs._ParentWindow)
				.InitialURL(InArgs._InitialURL)
				.ContentsToLoad(InArgs._ContentsToLoad)
				.ShowErrorMessage(InArgs._ShowErrorMessage)
				.SupportsTransparency(InArgs._SupportsTransparency)
				.SupportsThumbMouseButtonNavigation(InArgs._SupportsThumbMouseButtonNavigation)
				.BackgroundColor(InArgs._BackgroundColor)
				.PopupMenuMethod(InArgs._PopupMenuMethod)
				.ViewportSize(InArgs._ViewportSize)
				.OnLoadCompleted(OnLoadCompleted)
				.OnLoadError(OnLoadError)
				.OnLoadStarted(OnLoadStarted)
				.OnTitleChanged(OnTitleChanged)
				.OnUrlChanged(OnUrlChanged)
				.OnBeforePopup(OnBeforePopup)
				.OnCreateWindow(OnCreateWindow)
				.OnCloseWindow(OnCloseWindow)
				.OnUnhandledKeyDown(OnUnhandledKeyDown)
				.OnUnhandledKeyUp(OnUnhandledKeyUp)

				.OnBeforeNavigation(OnBeforeNavigation)
				.OnLoadUrl(OnLoadUrl)
				.OnShowDialog(OnShowDialog)
				.OnDismissAllDialogs(OnDismissAllDialogs)
				.Visibility(this, &SWebBrowserExtend::GetViewportVisibility)
				//.OnSuppressContextMenu(InArgs._OnSuppressContextMenu)
				//.OnDragWindow(InArgs._OnDragWindow)
				.BrowserFrameRate(InArgs._BrowserFrameRate)
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SCircularThrobber)
				.Radius(10.0f)
				.ToolTipText(LOCTEXT("LoadingThrobberToolTip", "Loading page..."))
				.Visibility(this, &SWebBrowserExtend::GetLoadingThrobberVisibility)
			]
		]
	];
}

void SWebBrowserExtend::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	
	if (HasMouseTransparency() && FSlateApplication::IsInitialized())
	{
		LastMousePixel = FLinearColor::Transparent;
		LastMouseTime = LastMouseTime + InDeltaTime;

		TSharedPtr<ICursor> Mouse = FSlateApplication::Get().GetPlatformCursor();
		if (Mouse.IsValid() && Mouse->GetType() != EMouseCursor::None)
		{
			FVector2D MousePosition = Mouse->GetPosition();
			if (!MousePosition.ContainsNaN())
			{
				FVector2D LocalMouse = AllottedGeometry.AbsoluteToLocal(MousePosition);
				FVector2D LocalSize = AllottedGeometry.GetLocalSize();

				FVector2D LocalUV = LocalSize.X > 0.0f && LocalSize.Y > 0.0f ? FVector2D(LocalMouse.X / LocalSize.X, LocalMouse.Y / LocalSize.Y) : FVector2D();

				if (LocalUV.X >= 0.0f && LocalUV.X <= 1.0f && LocalUV.Y >= 0.0f && LocalUV.Y <= 1.0f)
				{
					int32 X = FMath::FloorToInt(LocalUV.X * GetTextureWidth());
					int32 Y = FMath::FloorToInt(LocalUV.Y * GetTextureHeight());


					FLinearColor Pixel = ReadTexturePixel(X, Y);
					if ((Pixel.A < TransparencyThreadshold && LastMousePixel.A >= TransparencyThreadshold)
						|| (Pixel.A >= TransparencyThreadshold && LastMousePixel.A < TransparencyThreadshold))
					{
						LastMouseTime = 0.0f;
					}

					LastMousePixel = Pixel;
				}
				else
				{
					// UArrowComponent
					LastMousePixel = FLinearColor::White;
				}
			}
		}
	}
	else
	{
		LastMousePixel = FLinearColor::White;
	}

}

void SWebBrowserExtend::LoadURL(FString NewURL)
{
	if (BrowserView.IsValid())
	{
		BrowserView->LoadURL(NewURL);
	}
}

void SWebBrowserExtend::LoadString(FString Contents, FString DummyURL)
{
	if (BrowserView.IsValid())
	{
		BrowserView->LoadString(Contents, DummyURL);
	}
}

void SWebBrowserExtend::Reload()
{
	if (BrowserView.IsValid())
	{
		BrowserView->Reload();
	}
}

void SWebBrowserExtend::StopLoad()
{
	if (BrowserView.IsValid())
	{
		BrowserView->StopLoad();
	}
}

FText SWebBrowserExtend::GetTitleText() const
{
	if (BrowserView.IsValid())
	{
		return BrowserView->GetTitleText();
	}
	return LOCTEXT("InvalidWindow", "Browser Window is not valid/supported");
}

FString SWebBrowserExtend::GetUrl() const
{
	if (BrowserView.IsValid())
	{
		return BrowserView->GetUrl();
	}

	return FString();
}

FText SWebBrowserExtend::GetAddressBarUrlText() const
{
	if(BrowserView.IsValid())
	{
		return BrowserView->GetAddressBarUrlText();
	}
	return FText::GetEmpty();
}

bool SWebBrowserExtend::IsLoaded() const
{
	if (BrowserView.IsValid())
	{
		return BrowserView->IsLoaded();
	}

	return false;
}

bool SWebBrowserExtend::IsLoading() const
{
	if (BrowserView.IsValid())
	{
		return BrowserView->IsLoading();
	}

	return false;
}

bool SWebBrowserExtend::CanGoBack() const
{
	if (BrowserView.IsValid())
	{
		return BrowserView->CanGoBack();
	}
	return false;
}

void SWebBrowserExtend::GoBack()
{
	if (BrowserView.IsValid())
	{
		BrowserView->GoBack();
	}
}

FReply SWebBrowserExtend::OnBackClicked()
{
	GoBack();
	return FReply::Handled();
}

bool SWebBrowserExtend::CanGoForward() const
{
	if (BrowserView.IsValid())
	{
		return BrowserView->CanGoForward();
	}
	return false;
}

void SWebBrowserExtend::GoForward()
{
	if (BrowserView.IsValid())
	{
		BrowserView->GoForward();
	}
}

FReply SWebBrowserExtend::OnForwardClicked()
{
	GoForward();
	return FReply::Handled();
}

FText SWebBrowserExtend::GetReloadButtonText() const
{
	static FText ReloadText = LOCTEXT("Reload", "Reload");
	static FText StopText = LOCTEXT("StopText", "Stop");

	if (BrowserView.IsValid())
	{
		if (BrowserView->IsLoading())
		{
			return StopText;
		}
	}
	return ReloadText;
}

FReply SWebBrowserExtend::OnReloadClicked()
{
	if (IsLoading())
	{
		StopLoad();
	}
	else
	{
		Reload();
	}
	return FReply::Handled();
}

void SWebBrowserExtend::OnUrlTextCommitted( const FText& NewText, ETextCommit::Type CommitType )
{
	if(CommitType == ETextCommit::OnEnter)
	{
		LoadURL(NewText.ToString());
	}

	//UStaticMeshComponent
}

EVisibility SWebBrowserExtend::GetViewportVisibility() const
{
#pragma region extend

	if (!BrowserView.IsValid() || !BrowserView->IsInitialized())
	{
		// UE_LOG(LogTemp, Log, TEXT("Hidden"));
		return EVisibility::Hidden;
	}


	if (HasMouseTransparency() && LastMousePixel.A < TransparencyThreadshold && LastMouseTime >= TransparencyDelay)
	{
		// UE_LOG(LogTemp, Log, TEXT("HitTestInvisible ac = %d  LastMouseTime: %d") , LastMousePixel.A, LastMouseTime);
		
		//UGameplayStatics::GetPlayerControllerFromID(GetWorld(),0)->GameIN
		
		return EVisibility::SelfHitTestInvisible;
	}

	// UE_LOG(LogTemp, Log, TEXT("Visible"));

	return EVisibility::Visible;

#pragma endregion extend

}

EVisibility SWebBrowserExtend::GetLoadingThrobberVisibility() const
{
	if (bShowInitialThrobber && !BrowserView->IsInitialized())
	{
		return EVisibility::Visible;
	}
	return EVisibility::Hidden;
}




#pragma region Extend


FColor SWebBrowserExtend::ReadTexturePixel(int32 X, int32 Y) const
{
	if (X < 0 || X >= GetTextureWidth())
		return FColor::Transparent;
	if (Y < 0 || Y >= GetTextureHeight())
		return FColor::Transparent;

	TArray<FColor> Pixels = ReadTexturePixels(X, Y, 1, 1);
	if (Pixels.Num() > 0)
		return Pixels[0];

	return FColor::Transparent;
}

TArray<FColor> SWebBrowserExtend::ReadTexturePixels(int32 X, int32 Y, int32 Width, int32 Height) const
{
	TArray<FColor> OutPixels;
	if (!BrowserWindow.IsValid())
	{
		return OutPixels;
	}


	FSlateShaderResource* Resource = BrowserWindow->GetTexture();
	if (!Resource || Resource->GetType() != ESlateShaderResource::NativeTexture)
	{
		return OutPixels;
	}

	FTexture2DRHIRef TextureRHI;
	TextureRHI = ((TSlateTexture<FTexture2DRHIRef>*)Resource)->GetTypedResource();

	struct FReadSurfaceContext
	{
		FTexture2DRHIRef Texture;
		TArray<FColor>* OutData;
		FIntRect Rect;
		FReadSurfaceDataFlags Flags;
	};

	int32 ResourceWidth = (int32)Resource->GetWidth();
	int32 ResourceHeight = (int32)Resource->GetHeight();

	X = FMath::Clamp(X, 0, ResourceWidth - 1);
	Y = FMath::Clamp(Y, 0, ResourceHeight - 1);

	Width = FMath::Clamp(Width, 1, ResourceWidth);
	Width = Width - FMath::Max(X + Width - ResourceWidth, 0);

	Height = FMath::Clamp(Height, 1, ResourceHeight);
	Height = Height - FMath::Max(Y + Height - ResourceHeight, 0);

	FReadSurfaceContext Context =
	{
		TextureRHI,
		&OutPixels,
		FIntRect(X, Y, X + Width, Y + Height),
		FReadSurfaceDataFlags()
	};

	ENQUEUE_RENDER_COMMAND(ReadSurfaceCommand)(
		[Context](FRHICommandListImmediate& RHICmdList)
		{
			RHICmdList.ReadSurfaceData(
				Context.Texture,
				Context.Rect,
				*Context.OutData,
				Context.Flags
			);
		});

	FlushRenderingCommands();

	return OutPixels;
}

bool SWebBrowserExtend::HasMouseTransparency() const
{
	return bMouseTransparency && !bVirtualPointerTransparency;
}

bool SWebBrowserExtend::HasVirtualPointerTransparency() const
{
	return bVirtualPointerTransparency;
}

float SWebBrowserExtend::GetTransparencyDelay() const
{
	return TransparencyDelay;
}

float SWebBrowserExtend::GetTransparencyThreshold() const
{
	return TransparencyThreadshold;
}

int32 SWebBrowserExtend::GetTextureWidth() const
{
	if (!BrowserWindow.IsValid())
		return 0;

	FSlateShaderResource* Resource = BrowserWindow->GetTexture();
	if (!Resource)
		return 0;

	return Resource->GetWidth();
}

int32 SWebBrowserExtend::GetTextureHeight() const
{
	if (!BrowserWindow.IsValid())
		return 0;

	FSlateShaderResource* Resource = BrowserWindow->GetTexture();
	if (!Resource)
		return 0;

	return Resource->GetHeight();
}

#pragma endregion


void SWebBrowserExtend::ExecuteJavascript(const FString& ScriptText)
{
	if (BrowserView.IsValid())
	{
		BrowserView->ExecuteJavascript(ScriptText);
	}
}

void SWebBrowserExtend::GetSource(TFunction<void (const FString&)> Callback) const
{
	if (BrowserView.IsValid())
	{
		BrowserView->GetSource(Callback);
	}
}

void SWebBrowserExtend::BindUObject(const FString& Name, UObject* Object, bool bIsPermanent)
{
	if (BrowserView.IsValid())
	{
		BrowserView->BindUObject(Name, Object, bIsPermanent);
	}
}

void SWebBrowserExtend::UnbindUObject(const FString& Name, UObject* Object, bool bIsPermanent)
{
	if (BrowserView.IsValid())
	{
		BrowserView->UnbindUObject(Name, Object, bIsPermanent);
	}
}

void SWebBrowserExtend::BindAdapter(const TSharedRef<IWebBrowserAdapter>& Adapter)
{
	if (BrowserView.IsValid())
	{
		BrowserView->BindAdapter(Adapter);
	}
}

void SWebBrowserExtend::UnbindAdapter(const TSharedRef<IWebBrowserAdapter>& Adapter)
{
	if (BrowserView.IsValid())
	{
		BrowserView->UnbindAdapter(Adapter);
	}
}

void SWebBrowserExtend::BindInputMethodSystem(ITextInputMethodSystem* TextInputMethodSystem)
{
	if (BrowserView.IsValid())
	{
		BrowserView->BindInputMethodSystem(TextInputMethodSystem);
	}
}

void SWebBrowserExtend::UnbindInputMethodSystem()
{
	if (BrowserView.IsValid())
	{
		BrowserView->UnbindInputMethodSystem();
	}
}

#undef LOCTEXT_NAMESPACE

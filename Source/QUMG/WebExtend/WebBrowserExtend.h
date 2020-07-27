// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"

#include "WebBrowserExtend.generated.h"


class SWebBrowserExtend;
/**
 *
 */
UCLASS()
class QUMG_API UWebBrowserExtend : public UWidget
{
	GENERATED_UCLASS_BODY()



public:
	UFUNCTION(BlueprintCallable, Category = "UWebBrowser Extend")
		void BindUObject(FString Name, UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "UWebBrowser Extend")
		void DispatchEvent(const FString& eventName, const FString& paramjson);

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUrlChanged, const FText&, Text);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBeforePopup, FString, URL, FString, Frame);

	/**
	 * Load the specified URL
	 *
	 * @param NewURL New URL to load
	 */
	UFUNCTION(BlueprintCallable, Category = "Web Browser Extend")
		void LoadURL(const FString& NewURL);

	UFUNCTION(BlueprintCallable, Category = "Web Browser Extend")
		void SetInitialURL(const FString& NewURL);

	/**
	 * Load a string as data to create a web page
	 *
	 * @param Contents String to load
	 * @param DummyURL Dummy URL for the page
	 */
	UFUNCTION(BlueprintCallable, Category = "Web Browser Extend")
		void LoadString(const FString& Contents, const FString& DummyURL);

	/**
	* Executes a JavaScript string in the context of the web page
	*
	* @param ScriptText JavaScript string to execute
	*/
	UFUNCTION(BlueprintCallable, Category = "Web Browser Extend")
		void ExecuteJavascript(const FString& ScriptText);

	/**
	 * Get the current title of the web page
	 */
	UFUNCTION(BlueprintCallable, Category = "Web Browser Extend")
		FText GetTitleText() const;

	/**
	* Gets the currently loaded URL.
	*
	* @return The URL, or empty string if no document is loaded.
	*/
	UFUNCTION(BlueprintCallable, Category = "Web Browser Extend")
		FString GetUrl() const;

	/** Called when the Url changes. */
	UPROPERTY(BlueprintAssignable, Category = "Web Browser Extend | Event")
		FOnUrlChanged OnUrlChanged;

	/** Called when a popup is about to spawn. */
	UPROPERTY(BlueprintAssignable, Category = "Web Browser Extend | Event")
		FOnBeforePopup OnBeforePopup;
#pragma region event


public:
	DECLARE_EVENT(FString, FOnLoadCompleted)
	FOnLoadCompleted& OnLoadCompleted() { return this->Event_OnLoadCompleted; };

protected:
	FOnLoadCompleted Event_OnLoadCompleted;
#pragma endregion
protected:
	/**  */
	void HandleLoadCompleted();


public:

	//~ Begin UWidget interface
	virtual void SynchronizeProperties() override;
	// End UWidget interface

	/**  */
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	/** URL that the browser will initially navigate to. The URL should include the protocol, eg http:// */
	UPROPERTY(EditAnywhere, Category = Appearance)
		FString InitialURL;

	/** Should the browser window support transparency. */
	UPROPERTY(EditAnywhere, Category = Appearance)
		bool bSupportsTransparency = true;

	UPROPERTY(EditAnywhere, Category = Appearance)
		int BrowserFrameRate = 90;

	UPROPERTY(EditAnywhere, Category = Appearance)
	bool bMouseTransparency = false;
	UPROPERTY(EditAnywhere, Category = Appearance)
	bool bVirtualPointerTransparency = false;

	UPROPERTY(EditAnywhere, Category = Appearance)
	float TransparencyDelay = 0.0f;
	UPROPERTY(EditAnywhere, Category = Appearance)
	float TransparencyThreshold = 0.333f;

protected:
	TSharedPtr<SWebBrowserExtend> WebBrowserWidget;

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;


	//virtual FEventReply Native
	// End of UWidget interface

	void HandleOnUrlChanged(const FText& Text);
	bool HandleOnBeforePopup(FString URL, FString Frame);

	
};

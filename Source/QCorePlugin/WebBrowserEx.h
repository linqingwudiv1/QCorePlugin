// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WebBrowser.h"
#include "WebBrowserEx.generated.h"



/**
 * 
 */
UCLASS()
class QCOREPLUGIN_API UWebBrowserEx : public UWebBrowser
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UWebBrowserEx" )
	void BindUObject(FString Name, UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "UWebBrowserEx")
		void Init(const FString &Url = TEXT(""), bool bTransparency = false);
public: 
	void DispatchEvent(const FString &eventName, const FString &paramjson);

#pragma region event

public:
	DECLARE_EVENT(FString, FOnLoadCompleted)
	FOnLoadCompleted& OnLoadCompleted() { return this->Event_OnLoadCompleted; };
protected:
	FOnLoadCompleted Event_OnLoadCompleted;
#pragma endregion

public:

	//~ Begin UWidget interface
	virtual void SynchronizeProperties() override;
	// End UWidget interface

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

protected:
	void HandleLoadCompleted();
};

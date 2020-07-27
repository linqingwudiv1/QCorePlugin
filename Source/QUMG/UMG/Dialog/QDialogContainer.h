// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/Border.h"
#include "SlateCore/Public/Fonts/SlateFontInfo.h"
#include "QDialogContainer.generated.h"

/**
 * 
 */
//DECLARE_EVENT_OneParam(IAssetRegistry, FAssetAddedEvent, const FAssetData&);


UENUM(BlueprintType)
enum class EM_DialogScaleDirection : uint8
{
	EM_None = 0					UMETA(DisplayName = "None"),
	EM_Increase					UMETA(DisplayName = "Increase"),
	EM_Decrease					UMETA(DisplayName = "Decrease")
};

UCLASS(BlueprintType)
class QUMG_API UQDialogContainer : public UBorder
{
	GENERATED_UCLASS_BODY()

public:
	UQDialogContainer();

public:
	DECLARE_EVENT_ThreeParams(FString, FEvent_OnTitleMouseDown, UQDialogContainer*, const FGeometry&, const FPointerEvent&)
	FEvent_OnTitleMouseDown& OnTitleMouseDown() { return Event_OnTitleMouseDown; }
	
	DECLARE_EVENT_ThreeParams(FString, FEvent_OnTitleMouseUp, UQDialogContainer*, const FGeometry&, const FPointerEvent&)
	FEvent_OnTitleMouseUp& OnTitleMouseUp() { return Event_OnTitleMouseUp; }
	
	DECLARE_EVENT_ThreeParams(FString, FEvent_OnMouseMove, UQDialogContainer*, const FGeometry&, const FPointerEvent&)
	FEvent_OnMouseMove& OnMouseMove() { return Event_OnMouseMove; }
	
	//
	DECLARE_EVENT_ThreeParams(FString, FEvent_OnResizeSizeComplated, UQDialogContainer*, const FGeometry&, const FPointerEvent&)
	FEvent_OnResizeSizeComplated& OnResizeSizeComplated() { return Event_OnResizeSizeComplated; }
	
	//true is axis +, flase is axis -;
	DECLARE_EVENT_FiveParams(FString, FEvent_OnResizingSize, UQDialogContainer*, EM_DialogScaleDirection, EM_DialogScaleDirection, const FGeometry&, const FPointerEvent&)
	FEvent_OnResizingSize& OnResizingSize() { return Event_OnResizingSize; }
	
	DECLARE_EVENT_OneParam(FString, FEvent_OnClose, UQDialogContainer*)
	FEvent_OnClose& OnClose() { return Event_OnClose; }
	
#pragma region Event
//UInterface
		DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEvent_OnClose_BP, UQDialogContainer*, target);
		UPROPERTY(BlueprintAssignable, Category = "Button|Event")
			FEvent_OnClose_BP OnClose_BP;

#pragma endregion Event

private:
	FEvent_OnTitleMouseDown Event_OnTitleMouseDown;
	FEvent_OnTitleMouseUp Event_OnTitleMouseUp;
	FEvent_OnMouseMove Event_OnMouseMove;

	FEvent_OnResizingSize Event_OnResizingSize;
	FEvent_OnResizeSizeComplated Event_OnResizeSizeComplated;

	FEvent_OnClose Event_OnClose;

#if WITH_EDITOR
public:
	// UWidget interface
	virtual const FText GetPaletteCategory() override;
	// End UWidget interface
#endif

public:

	//左右
	TSharedPtr<SBorder> Border_ML;
	TSharedPtr<SBorder> Border_MR;

	//标题区
	TSharedPtr<SBorder> Border_MShortL;
	TSharedPtr<SBorder> Border_MShortR;

	//顶部		
	TSharedPtr<SBorder> Border_TL;
	TSharedPtr<SBorder> Border_TC;
	TSharedPtr<SBorder> Border_TR;

	//底部			 
	TSharedPtr<SBorder> Border_BL;
	TSharedPtr<SBorder> Border_BC;
	TSharedPtr<SBorder> Border_BR;


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QDialog Widget")
		/**  */
		FVector2D  MaxSize = FVector2D{ 1280.0f, 1024.0f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QDialog Widget")
		/**  */
		FVector2D  MinSize = FVector2D{ 320.0f, 480.0f };


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QDialog Widget")
		TSubclassOf<class UQDialogTitlebar> DefaultTitlebarClass;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QDialog Widget")
		/**  */
		FMargin DialogBorderSize = { 6.0f, 6.0f, 6.0f, 6.0f };

protected:
	/**  */
	virtual TSharedRef<SWidget> RebuildWidget() override;

	/**  */
	TSharedRef<SWidget>  CreateTitlebar();
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QDialog Widget")
		/**  */
		FSlateBrush TitleBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QDialog Widget")
		/**  */
		FSlateBrush BorderBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QDialog Widget")
		/**  */
		FSlateBrush BorderLineBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QDialog Widget")
		/**  */
		FLinearColor BorderColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QDialog Widget")
		/**  */
		FLinearColor TitleBrushColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QDialog Widget")
		/**  */
		FText TitleText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QDialog Widget")
		/**  */
		FSlateFontInfo DefaultFont;
	
protected:
	FReply HandleTitleBarMouseDown		(const FGeometry& geo, const FPointerEvent& ev);
	FReply HandleTitleBarMouseUp		(const FGeometry& geo, const FPointerEvent& ev);

	void HandleDialogTitleBarMouseDown	( const FGeometry& geo, const FPointerEvent& ev);
	void HandleDialogTitleBarMouseUp	( const FGeometry& geo, const FPointerEvent& ev);

	FReply HandleMouseMove				( const FGeometry& geo,	const FPointerEvent& ev );
	FReply HandleBorderAreaDown			( const FGeometry& geo,	const FPointerEvent& ev );
	FReply HandleBorderAreaUp			( const FGeometry& geo, const FPointerEvent& ev );

	FReply HandleCloseBtnClick();

	void HandleClose();
};
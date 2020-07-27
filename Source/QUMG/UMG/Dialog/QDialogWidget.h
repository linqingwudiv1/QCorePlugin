// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "umg/Dialog/QDialogContainer.h"
#include "QDialogWidget.generated.h"


/**
 * 
 */
UCLASS(BlueprintType)
class QUMG_API UQDialogWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "QDialog", Meta = (BindWidget) )
		class UCanvasPanel *DialogCanvas = nullptr;
		
	UPROPERTY(BlueprintReadOnly, Category = "QDialog", Meta = (BindWidget) )
		UBorder* Background = nullptr;
	
public:
protected:
#pragma region event
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual  TSharedRef<SWidget> RebuildWidget() override;
#pragma endregion event

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QDialog")
		/**  */
		bool bMove = false; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QDialog")
		/**  */
		bool bResizing = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QDialog")
		/**  */
		FVector2D vec2d_oldMousePos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QDialog")
		class UQDialogContainer *DialogContainer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QDialog")
		TArray< UQDialogContainer* > Arr_DialogContain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QDialog")
		EM_DialogScaleDirection XAxisDirection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QDialog")
		EM_DialogScaleDirection YAxisDirection;

#pragma region Event and Delegate
public:
	DECLARE_EVENT_OneParam(FString, FEvent_OnDialogClose, UQDialogContainer*)
	FEvent_OnDialogClose& OnClose() { return Event_OnClose; }

protected:
	FEvent_OnDialogClose Event_OnClose;
#pragma endregion

protected:
	/**  */
	void HandleOnTitleBarDown(class UQDialogContainer*, const FGeometry & geo, const FPointerEvent & ev);

	/**  */
	void HandleOnTitleBarUp(class UQDialogContainer*, const FGeometry & geo, const FPointerEvent & ev);

	/**  */
	void HandleOnMouseMove(class UQDialogContainer*, const FGeometry & geo, const FPointerEvent & ev);

	/**  */
	void HandleOnResizingDialog(class UQDialogContainer *target, EM_DialogScaleDirection XAsix, EM_DialogScaleDirection YAsix, const FGeometry & geo, const FPointerEvent & ev);
	/**  */
	void HandleOnResizingDialogComplate(class UQDialogContainer *target,  const FGeometry & geo, const FPointerEvent & ev);

	void HandleOnClose(class UQDialogContainer *target);

	void BindEvent(UQDialogContainer* const container);
};
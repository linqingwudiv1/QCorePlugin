// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "umg/Dialog/QDialogContainer.h"
#include "CppWgt_BaseDialog.generated.h"


/**
 * 
 */
UCLASS()
class QUMG_API UCppWgt_BaseDialog : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CppVariant")
		bool bMove = false; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CppVariant")
		bool bResizing = false;

	/**  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CppVariant")
		FVector2D vec2d_oldMousePos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CppVariant")
		class UQDialogContainer *DialogContainer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CppVariant")
		TArray< UQDialogContainer* > Arr_DialogContain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CppVariant")
		EM_DialogScaleDirection XAxisDirection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CppVariant")
		EM_DialogScaleDirection YAxisDirection;

#pragma region Event and Delegate
public:
	DECLARE_EVENT_OneParam(FString, FEvent_OnDialogClose, UQDialogContainer *)
	FEvent_OnDialogClose& OnClose() { return Event_OnClose; }

	/**
	* 用于BP,因为Delegate无法BP C++ 双端通用/( UFUNCTION 绑定可读性太差 )
	**/

	//FEvent_OnDialogClose_BP& OnClose_BP() { return Event_OnClose_BP; }

protected:
	FEvent_OnDialogClose Event_OnClose;
	//FEvent_OnDialogClose_BP Event_OnClose_BP;
#pragma endregion

protected:
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
protected:
	void HandleOnTitleBarDown(class UQDialogContainer*, const FGeometry & geo, const FPointerEvent & ev);
	void HandleOnTitleBarUp(class UQDialogContainer*, const FGeometry & geo, const FPointerEvent & ev);
	void HandleOnMouseMove(class UQDialogContainer*, const FGeometry & geo, const FPointerEvent & ev);

	void HandleOnResizingDialog(class UQDialogContainer*target, EM_DialogScaleDirection XAsix, EM_DialogScaleDirection YAsix, const FGeometry & geo, const FPointerEvent & ev);
	void HandleOnResizingDialogComplate(class UQDialogContainer *target,  const FGeometry & geo, const FPointerEvent & ev);


	virtual void HandleOnClose(class UQDialogContainer*target);
};
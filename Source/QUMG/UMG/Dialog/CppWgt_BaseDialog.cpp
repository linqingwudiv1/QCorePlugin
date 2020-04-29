// Fill out your copyright notice in the Description page of Project Settings.

#include "CppWgt_BaseDialog.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

#include "Kismet/GameplayStatics.h"
#include "UMG/Public/Blueprint/widgetblueprintlibrary.h"
#include "UMG/Public/Blueprint/WidgetLayoutLibrary.h"

#include "Kismet/KismetInputLibrary.h"
#include "Kismet/KismetMathLibrary.h"


#include "Runtime/Slate/Public/Widgets/Layout/SBorder.h"


void UCppWgt_BaseDialog::NativeOnInitialized() 
{
	Super::NativeOnInitialized();
}

void UCppWgt_BaseDialog::NativeConstruct()
{
	Super::NativeConstruct();

	UWidget* root_wgt	= this->GetRootWidget();
	UClass*  temp_class = root_wgt->GetClass();

	UCanvasPanel* root_canvas = Cast< UCanvasPanel >(root_wgt);
	check ( root_canvas );

	if ( root_canvas == nullptr )
	{
		UE_LOG(LogTemp, Error, TEXT("[Error]: root is not UCanvasPanel !!!! "));
		return;
	}

	TArray<UWidget *> children = root_canvas->GetAllChildren();

	for (auto wgt_item : children)
	{
		auto temp_item = Cast< UQDialogContainer >( wgt_item );

		if (temp_item != nullptr)
		{
			Arr_DialogContain.Add(temp_item);

			UCanvasPanelSlot* temp_canvasSlot = Cast<UCanvasPanelSlot>(temp_item->Slot);

			
			//UE_LOG(LogTemp, Log,TEXT("[desired Size:] %s"));
			if ( temp_canvasSlot->GetSize().X < temp_item->GetDesiredSize().X ||
				 temp_canvasSlot->GetSize().Y < temp_item->GetDesiredSize().Y    )
			{
				FVector2D temp_size = temp_canvasSlot->GetSize();
				temp_size.X = FMath::Max<float>( temp_canvasSlot->GetSize().X, temp_item->GetDesiredSize().X );
				temp_size.Y = FMath::Max<float>( temp_canvasSlot->GetSize().Y, temp_item->GetDesiredSize().Y );
			}

			temp_item->OnTitleMouseDown()		.AddUObject(this, &UCppWgt_BaseDialog::HandleOnTitleBarDown);
			temp_item->OnTitleMouseUp()			.AddUObject(this, &UCppWgt_BaseDialog::HandleOnTitleBarUp);
			temp_item->OnClose()				.AddUObject(this, &UCppWgt_BaseDialog::HandleOnClose);

			temp_item->OnResizingSize()			.AddUObject(this, &UCppWgt_BaseDialog::HandleOnResizingDialog);
			temp_item->OnResizeSizeComplated()	.AddUObject(this, &UCppWgt_BaseDialog::HandleOnResizingDialogComplate);
		}
	}
}

void UCppWgt_BaseDialog::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	//处理位移和缩放
	APlayerController *controller = UGameplayStatics::GetPlayerController(this, 0);

	if (controller == nullptr) 
	{
		return;
	}

	{
		FVector2D newMousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);

		if ( bMove )
		{
			UCanvasPanelSlot* canvas = Cast<UCanvasPanelSlot>(DialogContainer->Slot);

			if (canvas != nullptr) 
			{
				auto temp_vec = newMousePos - vec2d_oldMousePos;
				canvas->SetPosition(canvas->GetPosition() + temp_vec);
			}
			
			vec2d_oldMousePos = newMousePos;

		}

		if ( bResizing )
		{
			UCanvasPanelSlot* canvas = Cast<UCanvasPanelSlot>(DialogContainer->Slot);

			if (canvas == nullptr)
			{
				UE_LOG(LogTemp, Log, TEXT("[dialog warning]: canvas is not vaild."));
				return;
			}

			FVector2D offset_vec = newMousePos - vec2d_oldMousePos;
			
			FVector2D canvasPos  = canvas->GetPosition();
			FVector2D canvasSize = canvas->GetSize();
			

			FVector2D newPos = canvasPos;
			FVector2D newSize = canvasSize;

			switch ( XAxisDirection )
			{
				case EM_DialogScaleDirection::EM_None :
				{
					newPos.X  = canvasPos.X;
					newSize.X = canvasSize.X;
					break;
				}
				case EM_DialogScaleDirection::EM_Increase :
				{
					newPos.X  = canvasPos.X ;
					newSize.X = canvasSize.X + offset_vec.X;

					break;
				}
				case EM_DialogScaleDirection::EM_Decrease :
				{
					newPos.X  = canvasPos .X + offset_vec.X ;
					newSize.X = canvasSize.X - offset_vec.X ;
					
					break;
				}
			}
			
			switch ( YAxisDirection )
			{
				case EM_DialogScaleDirection::EM_None: 
				{
					newPos.Y	 = canvasPos.Y;
					canvasSize.Y = canvasPos.Y;
					break;
				}
				case EM_DialogScaleDirection::EM_Increase:
				{
					newPos .Y = canvasPos.Y;
					newSize.Y = canvasSize.Y + offset_vec.Y;
					break;
				}
				case EM_DialogScaleDirection::EM_Decrease:
				{
					newPos .Y = canvasPos. Y + offset_vec.Y;
					newSize.Y = canvasSize.Y - offset_vec.Y;
					break;
				}
			}

			canvas->SetPosition(newPos);
			canvas->SetSize(newSize);

			vec2d_oldMousePos = newMousePos;
		}
	}
}

FReply UCppWgt_BaseDialog::NativeOnMouseButtonUp(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	if ( bResizing ) 
	{
		bResizing = false;
		return FReply::Handled();
	}
	else 
	{
		return FReply::Unhandled();
	}

}

void UCppWgt_BaseDialog::HandleOnTitleBarDown(UQDialogContainer *target, const FGeometry & geo, const FPointerEvent & ev)
{
	bMove = true;
	DialogContainer = target;
	if (target->Border_ML->IsHovered() || target->Border_MR->IsHovered())
	{
		bResizing = true;
	}

	APlayerController* controller = UGameplayStatics::GetPlayerController(this, 0);
	vec2d_oldMousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);

	FInputModeUIOnly mode;
	controller->SetInputMode(mode);
}

void UCppWgt_BaseDialog::HandleOnTitleBarUp(UQDialogContainer *target, const FGeometry & geo, const FPointerEvent & ev)
{
	bMove = false;
	APlayerController *controller = UGameplayStatics::GetPlayerController(this, 0);
	FInputModeGameAndUI mode;

	controller->SetInputMode(mode);
}

void UCppWgt_BaseDialog::HandleOnMouseMove(UQDialogContainer *target, const FGeometry & geo, const FPointerEvent & ev)
{

}

void UCppWgt_BaseDialog::HandleOnResizingDialog( UQDialogContainer * target, 
												 EM_DialogScaleDirection XAsix , 
												 EM_DialogScaleDirection YAsix , 
												 const FGeometry &geo , 
												 const FPointerEvent &ev )
{
	DialogContainer = target;
	bResizing = true;
	XAxisDirection = XAsix;
	YAxisDirection = YAsix;

	APlayerController* controller = UGameplayStatics::GetPlayerController(this, 0);
	vec2d_oldMousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);

	FInputModeUIOnly mode;
	controller->SetInputMode(mode);

}

void UCppWgt_BaseDialog::HandleOnResizingDialogComplate(UQDialogContainer * target, const FGeometry & geo, const FPointerEvent & ev)
{
	bResizing = false;

	APlayerController *controller = UGameplayStatics::GetPlayerController(this, 0);
	FInputModeGameAndUI mode;

	controller->SetInputMode(mode);
}

void UCppWgt_BaseDialog::HandleOnClose(UQDialogContainer *target)
{
	target->RemoveFromParent();
	Arr_DialogContain.Remove(target);

	if (Arr_DialogContain.Num() <= 0 ) 
	{
		this->RemoveFromParent();
	}

	this->OnClose().Broadcast(target);
}
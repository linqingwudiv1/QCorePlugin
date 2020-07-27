// Fill out your copyright notice in the Description page of Project Settings.

#include "QDialogWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

#include "Kismet/GameplayStatics.h"
#include "UMG/Public/Blueprint/widgetblueprintlibrary.h"
#include "UMG/Public/Blueprint/WidgetLayoutLibrary.h"

#include "Kismet/KismetInputLibrary.h"
#include "Kismet/KismetMathLibrary.h"


#include "Runtime/Slate/Public/Widgets/Layout/SBorder.h"



UQDialogWidget::UQDialogWidget( const FObjectInitializer & PCIP)
 :Super(PCIP)
{

}

void UQDialogWidget::NativeOnInitialized() 
{
	Super::NativeOnInitialized();
}

void UQDialogWidget::NativeConstruct()
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

	for (UWidget* wgt_item : children)
	{
		UQDialogContainer* dialog_item = Cast< UQDialogContainer >( wgt_item );

		if (dialog_item != nullptr)
		{
			Arr_DialogContain.Add(dialog_item);

			UCanvasPanelSlot* temp_canvasSlot = Cast<UCanvasPanelSlot>(dialog_item->Slot);
			
			//UE_LOG(LogTemp, Log,TEXT("[desired Size:] %s"));
			if ( temp_canvasSlot->GetSize().X < dialog_item->GetDesiredSize().X ||
				 temp_canvasSlot->GetSize().Y < dialog_item->GetDesiredSize().Y    )
			{
				FVector2D temp_size = temp_canvasSlot->GetSize();
				temp_size.X = FMath::Max<float>( temp_canvasSlot->GetSize().X, dialog_item->GetDesiredSize().X );
				temp_size.Y = FMath::Max<float>( temp_canvasSlot->GetSize().Y, dialog_item->GetDesiredSize().Y );
			}
			
			this->BindEvent(dialog_item);
		}
	}
}

void UQDialogWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	//处理位移和缩放

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
				case EM_DialogScaleDirection::EM_Increase :
				{
					// 正方向递增(右边拖拽)
					newPos.X  = canvasPos.X ;
					newSize.X = FMath::Clamp(canvasSize.X + offset_vec.X, DialogContainer->MinSize.X, DialogContainer->MaxSize.X);
					break;
				}
				case EM_DialogScaleDirection::EM_Decrease :
				{
					float real_width = FMath::Clamp(canvasSize.X - offset_vec.X, DialogContainer->MinSize.X, DialogContainer->MaxSize.X);
					
					float difference_offset = real_width - ( canvasSize.X - offset_vec.X );
					
					if (difference_offset == 0.0f)
					{
						newPos.X  = canvasPos.X  + offset_vec.X;
						newSize.X = canvasSize.X - offset_vec.X;
					}
					
					break;
				}
				default:{ break; }
			}
			
			switch ( YAxisDirection )
			{
				case EM_DialogScaleDirection::EM_Increase:
				{
					newPos .Y = canvasPos.Y;

					newSize.Y = FMath::Clamp(canvasSize.Y + offset_vec.Y, DialogContainer->MinSize.Y, DialogContainer->MaxSize.Y);
					break;
				}
				case EM_DialogScaleDirection::EM_Decrease:
				{
					// step one: count offset value.`
					float real_height = FMath::Clamp(canvasSize.Y - offset_vec.Y, DialogContainer->MinSize.Y, DialogContainer->MaxSize.Y);

					float difference_offset = real_height - (canvasSize.Y - offset_vec.Y);
					if (difference_offset == 0.0f)
					{
						newPos.Y = canvasPos.Y + offset_vec.Y;
						newSize.Y = canvasSize.Y - offset_vec.Y;
					}

					break;
				}
				default: { break; }
			}

			canvas->SetPosition(newPos);
			canvas->SetSize(newSize);

			vec2d_oldMousePos = newMousePos;
		}
	}
}

FReply UQDialogWidget::NativeOnMouseButtonUp(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent)
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

TSharedRef<SWidget> UQDialogWidget::RebuildWidget()
{
	return Super::RebuildWidget();
}

void UQDialogWidget::HandleOnTitleBarDown(UQDialogContainer *target, const FGeometry & geo, const FPointerEvent & ev)
{
	bMove = true;
	DialogContainer = target;
	
	if (target->Border_ML->IsHovered() || target->Border_MR->IsHovered())
	{
		bResizing = true;
	}

	APlayerController* controller = UGameplayStatics::GetPlayerController(this, 0);
	vec2d_oldMousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
}

void UQDialogWidget::HandleOnTitleBarUp(UQDialogContainer *target, const FGeometry & geo, const FPointerEvent & ev)
{
	bMove = false;
	APlayerController *controller = UGameplayStatics::GetPlayerController(this, 0);
}

void UQDialogWidget::HandleOnMouseMove(UQDialogContainer *target, const FGeometry & geo, const FPointerEvent & ev)
{
}

void UQDialogWidget::HandleOnResizingDialog( UQDialogContainer * target		, 
											 EM_DialogScaleDirection XAsix	, 
											 EM_DialogScaleDirection YAsix	, 
											 const FGeometry &geo			, 
											 const FPointerEvent &events	 )
{
	DialogContainer = target;
	bResizing = true;
	XAxisDirection = XAsix;
	YAxisDirection = YAsix;

	APlayerController* controller = UGameplayStatics::GetPlayerController(this, 0);

	vec2d_oldMousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
}

void UQDialogWidget::HandleOnResizingDialogComplate(UQDialogContainer * target, const FGeometry & geo, const FPointerEvent & ev)
{
	bResizing = false;
	APlayerController *controller = UGameplayStatics::GetPlayerController(this, 0);
}

void UQDialogWidget::HandleOnClose(UQDialogContainer *target)
{
	target->RemoveFromParent();
	Arr_DialogContain.Remove(target);
	
	if (Arr_DialogContain.Num() <= 0 ) 
	{
		this->RemoveFromParent();
	}

	this->OnClose().Broadcast(target);
}

void UQDialogWidget::BindEvent(  UQDialogContainer  * const container )
{

	container->OnTitleMouseDown().AddUObject(this, &UQDialogWidget::HandleOnTitleBarDown);
	container->OnTitleMouseUp().AddUObject(this, &UQDialogWidget::HandleOnTitleBarUp);
	container->OnClose().AddUObject(this, &UQDialogWidget::HandleOnClose);

	container->OnResizingSize().AddUObject(this, &UQDialogWidget::HandleOnResizingDialog);
	container->OnResizeSizeComplated().AddUObject(this, &UQDialogWidget::HandleOnResizingDialogComplate);
}

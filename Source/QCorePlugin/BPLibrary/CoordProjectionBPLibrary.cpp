// Fill out your copyright notice in the Description page of Project Settings.

#include "CoordProjectionBPLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"

#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "CoordProjectionBPLibrary.h"
#include "Engine/Engine.h"

#include "SceneView.h"
#include "UObject/UObjectIterator.h"

FVector2D UCoordProjectionBPLibrary::ProjectWorldToScreenPosition(const FVector& WorldLocation)
{
	TObjectIterator<APlayerController> ThePC;

	if (!ThePC) 
	{
		return FVector2D::ZeroVector;
	} 

	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(ThePC->Player);
	if (LocalPlayer != NULL && LocalPlayer->ViewportClient != NULL && LocalPlayer->ViewportClient->Viewport != NULL)
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// Create A View family for the game viewport
		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
			LocalPlayer->ViewportClient->Viewport,
			ThePC->GetWorld()->Scene,
			LocalPlayer->ViewportClient->EngineShowFlags)
			.SetRealtimeUpdate(true));

		// Calculate a view where the player is to update the streaming from the players start location
		FVector ViewLocation;
		FRotator ViewRotation;
		FSceneView* SceneView = LocalPlayer->CalcSceneView(&ViewFamily, /*out*/ ViewLocation, /*out*/ ViewRotation, LocalPlayer->ViewportClient->Viewport);

		//Valid Scene View?
		if (SceneView)
		{
			//Return
			FVector2D ScreenLocation;
			SceneView->WorldToPixel(WorldLocation, ScreenLocation);

			return ScreenLocation;
		}
	}

	return FVector2D::ZeroVector;
}


bool UCoordProjectionBPLibrary::DeprojectScreenPosition(UObject * WorldContextObject, const FVector2D & ViewportPosition, FVector & OutWorldOrigin, FVector & OutWorldDirection)
{
	bool bResult = false;

	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
	{
		bResult = UGameplayStatics::DeprojectScreenToWorld(World->GetFirstPlayerController(), ViewportPosition, OutWorldOrigin, OutWorldDirection);
	}

	return bResult;
}



TArray<FVector2D> UCoordProjectionBPLibrary::WorldBoxToScreenBox(UObject *WorldContextObject, const FVector& Center, const FRotator& rot, const FVector& Extend)
{
	
	
	FTransform tran(rot);
	TArray<FVector> arr;

	arr.Add(Center + tran.TransformPosition(FVector{  Extend.X ,  Extend.Y,  Extend.Z }));
	arr.Add(Center + tran.TransformPosition(FVector{  Extend.X , -Extend.Y, -Extend.Z }));
	arr.Add(Center + tran.TransformPosition(FVector{  Extend.X ,  Extend.Y, -Extend.Z }));
	arr.Add(Center + tran.TransformPosition(FVector{  Extend.X , -Extend.Y,  Extend.Z }));

	arr.Add(Center + tran.TransformPosition(FVector{ -Extend.X ,  Extend.Y,  Extend.Z }));
	arr.Add(Center + tran.TransformPosition(FVector{ -Extend.X , -Extend.Y, -Extend.Z }));
	arr.Add(Center + tran.TransformPosition(FVector{ -Extend.X ,  Extend.Y, -Extend.Z }));
	arr.Add(Center + tran.TransformPosition(FVector{ -Extend.X , -Extend.Y,  Extend.Z }));

	TArray<FVector2D> arr2d;

	for (FVector vec : arr)
	{
		//DrawDebugPoint(WorldContextObject->GetWorld(), vec, 4, FColor::Red);
		FVector2D temp_vec2d = UCoordProjectionBPLibrary::ProjectWorldToScreenPosition(vec) * (1.00000f / UWidgetLayoutLibrary::GetViewportScale(WorldContextObject));
		arr2d.Add(temp_vec2d);
	}

	TArray<FVector2D> arrline;

#pragma region draw surface line 

	arrline.Add(arr2d[0]);
	arrline.Add(arr2d[2]);

	arrline.Add(arr2d[1]);
	arrline.Add(arr2d[3]);

	arrline.Add(arr2d[4]);
	arrline.Add(arr2d[6]);

	arrline.Add(arr2d[5]);
	arrline.Add(arr2d[7]);

	arrline.Add(arr2d[0]);
	arrline.Add(arr2d[4]);

	arrline.Add(arr2d[3]);
	arrline.Add(arr2d[7]);

	arrline.Add(arr2d[1]);
	arrline.Add(arr2d[5]);

	arrline.Add(arr2d[2]);
	arrline.Add(arr2d[6]);

	arrline.Add(arr2d[0]);
	arrline.Add(arr2d[3]);

	arrline.Add(arr2d[2]);
	arrline.Add(arr2d[1]);

	arrline.Add(arr2d[6]);
	arrline.Add(arr2d[5]);

	arrline.Add(arr2d[4]);
	arrline.Add(arr2d[7]);
#pragma endregion 
	
	return arrline;
}


#include "Kismet/KismetInputLibrary.h"

FVector2D UCoordProjectionBPLibrary::GetMousePosition(UObject * WorldContextObject)
{
	if (!GEngine) 
	{
		return FVector2D{};
	}
	UGameViewportClient* viewport = GEngine->GameViewport;
	float factory = UWidgetLayoutLibrary::GetViewportScale(WorldContextObject);
	FVector2D mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(WorldContextObject);
	mousePos = mousePos * factory;
	return mousePos;
}

bool UCoordProjectionBPLibrary::GetMousePositionByScreenPosition(UObject * WorldContextObject,  FVector & OutWorldOrigin, FVector & OutWorldDirection)
{
	FVector2D ViewPos = UCoordProjectionBPLibrary::GetMousePosition(WorldContextObject);
	bool bResult = UCoordProjectionBPLibrary::DeprojectScreenPosition(WorldContextObject, ViewPos, OutWorldOrigin, OutWorldDirection);
	return bResult;
}
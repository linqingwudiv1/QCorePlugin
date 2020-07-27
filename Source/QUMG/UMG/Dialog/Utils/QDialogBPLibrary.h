// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "QDialogBPLibrary.generated.h"

/**
 * 
 */
UCLASS()
class QUMG_API UQDialogBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "QDialog | Helper")
		static void CreateDialog(TSubclassOf<UUserWidget> ContentClass,const FVector2D &position );
};

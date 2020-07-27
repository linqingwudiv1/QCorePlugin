// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/Interface.h"

#include "IQDialogInterface.generated.h"

UINTERFACE(BlueprintType, Meta= (CannotImplementInterfaceInBlueprint))
class QUMG_API UQDialogInterface : public  UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class QUMG_API IQDialogInterface : public IInterface
{
	GENERATED_IINTERFACE_BODY()
	/**  */
public:

	UFUNCTION(BlueprintCallable ,Category = "QDialog" )
	virtual void Close () {}


	//virtual void Close_Implementation();

	//UFUNCTION(BlueprintNativeEvent, Category = "QDialog")
	//void Maximalize();
	////virtual void Maximalize_Implementation();
	//
	//UFUNCTION(BlueprintNativeEvent,Category = "QDialog")
	//void Minimalize();
	//virtual void Minimalize_Implementation();

};



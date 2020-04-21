// Fill out your copyright notice in the Description page of Project Settings.


#include "NamedPipeHelper.h"

UNamedPipeHelper::UNamedPipeHelper()
{
	this->AddToRoot();
}

UNamedPipeHelper * UNamedPipeHelper::CreateNamedPipeHelper(const FString& PipeName, bool bServer, bool bAsync)
{
	UNamedPipeHelper * Obj = NewObject<UNamedPipeHelper>();

	if (Obj == nullptr || !Obj->IsValidLowLevel()) 
	{
		return nullptr;
	}

	Obj->PipeName = PipeName;
	Obj->bServer = bServer;
	Obj->bAsync = bAsync;

	return Obj;
}

FPlatformNamedPipe * UNamedPipeHelper::GetNamedPipe()
{
	//static bCreate
	if (!NamedPipe.IsCreated())
	{
		bool isSuccess = NamedPipe.Create(this->PipeName, bServer, bAsync);

		if (!isSuccess)
		{
			return nullptr;
		}
	}
	return &NamedPipe;
}


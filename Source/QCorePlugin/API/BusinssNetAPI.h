// L.Q 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "API/NetAPI.h"
#include "BusinssNetAPI.generated.h"

/**
 * 
 */
UCLASS()
class QCOREPLUGIN_API UBusinssNetAPI : public UObject
{
	GENERATED_BODY()
public:
	static FHttpRequestPtr UploadImage();
};
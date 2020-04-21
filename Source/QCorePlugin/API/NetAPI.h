// L.Q 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Http.h"
#include "NetAPI.generated.h"

/**
 * 
 */
UCLASS()
class QCOREPLUGIN_API UNetAPI : public UObject
{
	GENERATED_BODY()
public:
	//Setting.eg timeout/delay time/once read max buffer
	static FHttpModule* GetHttpModule();

	static FHttpManager& GetHttpMgr();
	//UFUNCTION()
	static TSharedRef<IHttpRequest> createRequst(FString _url,
		FString _method = TEXT("GET"),
		FString _content = TEXT(""),
		FString _contentType = TEXT("text/html;charset=UTF-8"));

	static TSharedRef<IHttpRequest> createUploadImageRequest(FString _url, TArray<uint8>& rawData);
};


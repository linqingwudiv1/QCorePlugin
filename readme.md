## Description

support 4.2x.x and 5.3.x


Tool Plugin.The UE4 Feature Extend/Helper contain some method about Texture/Pipe/WebBrowser/HTTP/IO/JSON/round corner in 4.2x.x(ue5 already implement round feature UBorder)


5.3.x new:

* lumen high frame ouput and high shot to disk(like scene capture 2d)
* winapi implmentantion system open file dialog at runtime.
* Additional Examples


# Use Exam

##### 1.High Shot and open file dialog at runtime :

![描述文本](Readme/BPExam.jpg)

##### 2.umg splitter component widget:

<img src="Readme/spliterwidget.png" alt="描述文本" width="720"/>
<img src="Readme/splitteranim.gif" alt="描述文本" width="480"/>


##### 3. download large file use range download to disk:

```cpp
UDownloadHelper* UHttpBPLibrary::DownloadRange(UObject* WorldContextObject, const FString& Url)
{
	UDownloadHelper* NewObj = NewObject<UDownloadHelper>(WorldContextObject, NAME_None, RF_Transient);
	
	NewObj->StartByDisk(Url, TEXT("d:/disk.png"));

	NewObj->OnDownloadCompleted().AddLambda([](UDownloadHelper* _DownloadHelper, bool bSuccessful , const FString &msg)
		{
			UE_LOG(LogTemp, Log, TEXT("Download Completed... %d %s"), bSuccessful ,*msg);
		});
	NewObj->OnDownloadProgress().AddLambda([](UDownloadHelper* _DownloadHelper,const FQHttpProgress& info )
		{
			UE_LOG(LogTemp, Log, TEXT("Download Progress... %d / %d   %f / %f"), info.NewReceive, info.NewSent, info.ReceiveProgress, info.SentProgress);
		});

	return NewObj;
}
```

#### 4. dynamic loading texture2d and generate mipmap array:

```cpp
	auto imageHelper = NewObject<UImageHelper>();
	//generate mipmaps array need ensure image size is 2 of power or enable bForceGenerateMips
	UTexture2D* t2d = imageHelper->LoadFromDisk(const FString &Path, bool  bAutoGenerateMips = true, bool bForceGenerateMips = false);
```

Copy to QCorePlugin by 2024

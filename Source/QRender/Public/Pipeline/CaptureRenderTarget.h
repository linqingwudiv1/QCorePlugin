/**
 * 
 * 
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DTO/ImageFormatEx.h"
#include "CaptureRenderTarget.generated.h"


class APostProcessVolume;


UENUM()
enum class ECaptureState : uint8
{
	Initilize = 0,
	Warmup,
	Renderering,
	Renderered,
	Captured,
	Finished
};

UENUM(BlueprintType)
enum class ECaptureCubeFace : uint8
{
	CubeFace_PosX = 0,
	CubeFace_NegX,
	CubeFace_PosY,
	CubeFace_NegY,
	CubeFace_PosZ,
	CubeFace_NegZ,
	CubeFace_MAX
};

/**
 * XXX:维护自己的worldTime用于Vedio
 */
UCLASS(BlueprintType)
class QRENDER_API UCaptureRenderTarget : public UObject, public FTickableGameObject
{
	GENERATED_UCLASS_BODY()
public:
	//UCaptureRenderTarget();
	~UCaptureRenderTarget();

	UFUNCTION(BlueprintCallable)
	void Initilize(const FIntPoint& Size,
	               const FVector& Location = FVector::ZeroVector,
	               const FRotator& Rotator = FRotator::ZeroRotator,
	               const float FOV = 90.0f,
	               const float ScenePercentage = 100.0f,
	               const bool InbNotInverseGamma = false,
	               const bool bInAsync = true, //是否是异步捕获模式
	               APostProcessVolume* PP = nullptr);

	UFUNCTION(BlueprintCallable)
	void ResizeRenderTarget(const FIntPoint& Size); 

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsInitilized()
	{
		return GetState() != ECaptureState::Initilize;
	}
	
	UFUNCTION(BlueprintCallable)
	void SetScreenPercentage(float screenPercentage = 100.0f);

	UFUNCTION(BlueprintCallable)
		void SetWorldPause(bool NewPause);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UTextureRenderTarget2D* GetRTT2D() const
	{
		return this->mRTT2D;
	}

	UFUNCTION(BlueprintCallable)
	void Capture(const uint8 PreCaptureFrameNum = 2);

	UFUNCTION(BlueprintCallable)
	void CaptureFromLocationAndRotation(const FVector& Location = FVector::ZeroVector,
	                                    const FRotator& Rotator = FRotator::ZeroRotator,
	                                    const uint8 PreCaptureFrameNum = 2
	);


	//UFUNCTION(BlueprintCallable)
	void SetupCustomPassMass(FSceneView* InSceneView);

	//UFUNCTION(BlueprintCallable)
	//void AddWeightedBlendables(TArray<FWeightedBlendable>& arr);

	UFUNCTION(BlueprintCallable)
	void SaveToDisk(FString path = TEXT(""), ECaptureCubeFace CubeFace = ECaptureCubeFace::CubeFace_MAX, EImageFormatEx ImageFormat = EImageFormatEx::PNG, int Quility = 100);


	UFUNCTION(BlueprintCallable)
	void SaveToDiskAsync(FString path = TEXT(""), ECaptureCubeFace CubeFace = ECaptureCubeFace::CubeFace_MAX, EImageFormatEx ImageFormat = EImageFormatEx::PNG, int Quility = 100);

	ECaptureState GetState();
#pragma region event
public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCaptureCompleted, UCaptureRenderTarget*)

	FOnCaptureCompleted& OnCaptureCompleted()
	{
		return Event_OnCaptureCompleted;
	}

	FOnCaptureCompleted& OnCaptureCompleted_RenderThread()
	{
		return Event_OnCaptureCompleted_RenderThread;
	}

	//FOnCaptureCompleted& OnCaptureWarmupCompleted()
	//{
	//	return Event_OnCaptureWarmupCompleted;
	//}

	FOnCaptureCompleted& OnCaptureWarmupCompleted_RenderThread()
	{
		return Event_OnCaptureWarmupCompleted_RenderThread;
	}

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSaveToDiskFinished, const FString &)
	FOnSaveToDiskFinished& OnSaveToDiskFinished()
	{
		check(IsInGameThread());
		return Event_OnSaveToDiskFinished;
	}

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveToDiskFinished_BP,const FString&, SavePath);
	UPROPERTY(BlueprintAssignable)
	FOnSaveToDiskFinished_BP OnSaveToDiskFinished_BP;
protected:
	FOnCaptureCompleted Event_OnCaptureCompleted;
	FOnCaptureCompleted Event_OnCaptureCompleted_RenderThread;
	//FOnCaptureCompleted Event_OnCaptureWarmupCompleted;
	FOnCaptureCompleted Event_OnCaptureWarmupCompleted_RenderThread;
	FOnSaveToDiskFinished Event_OnSaveToDiskFinished;
#pragma endregion event
protected:
	void InitRenderTarget(const FIntPoint& Size);
	void SetState(ECaptureState NewState);
	TSharedPtr<FSceneViewFamilyContext> SetupViewFamily();
	FSceneView* SetupSceneView(TSharedPtr<FSceneViewFamilyContext> InViewFamily);

	void UpdateSceneViewPP(FSceneView* SceneView);
	void CalculateProjectionMatrix();

	//ref: UMovieScenePipeline::FlushAsyncEngineSystems 同步等待UE完成渲染每一帧前的预处理..
	void FlushAsyncEngineSystems();
	void RenderFrame();
	void PostRendererSubmission();
	void HandleFrame_OnRenderThread();
protected:
	#pragma region tickable impl
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override { return TStatId(); }
	#pragma endregion tickable impl

public:
	/** Read RTT pixel Data, from FloatRGBA to R8G8B8A */
	static bool GetRawData(UTextureRenderTarget2D* RTT, TArray64<uint8>& OutRawData, ECaptureCubeFace CubeFace = ECaptureCubeFace::CubeFace_MAX);
private:
	bool IOToDisk(const TArray64<uint8> & rawData,const FString& SavePath, const FIntPoint &Size, EImageFormatEx ImageFormat, int Quility);
protected:
#pragma region View
	FMatrix mProjectionMatrix = FMatrix::Identity;
	FVector mViewLocation = FVector::ZeroVector;
	FRotator mViewRotation = FRotator::ZeroRotator;
	float mViewFOV = 90.0f;
	float mScenePercentage = 100.0f; // important 
	bool bWorldPause = false;
	//bool bFixedExposure = false;

#pragma endregion
	bool bNotInverseGamma = false;
	bool bAsync = true;

	//Only Sync Mode
	bool bStartWarmup = false;
	bool bStartRender = false;

	/**
	 * 
	 */
	FSceneViewStateReference SceneViewState;

	UPROPERTY()
	UTextureRenderTarget2D* mRTT2D = nullptr;

	ECaptureState CaptureState = ECaptureState::Initilize;

	//预渲染帧数
	uint8 PreRenderingFrameNum = 2;
	uint8 CurPreFrame = 0;

	UPROPERTY()
	APostProcessVolume* mPP = nullptr;

	UPROPERTY()
	class UMaterial* Mat_PPGamma;
	
	UPROPERTY()
	class UMaterialInstanceDynamic* MatInsDyn_PPGamma;
};

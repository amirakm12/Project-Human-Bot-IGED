#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "Http.h"
#include "Json.h"
#include "IGEDModelManager.generated.h"

UENUM(BlueprintType)
enum class EModelType : uint8
{
    GPT4          UMETA(DisplayName = "GPT-4"),
    GPT4Turbo     UMETA(DisplayName = "GPT-4 Turbo"),
    GPT35Turbo    UMETA(DisplayName = "GPT-3.5 Turbo"),
    Claude3Opus   UMETA(DisplayName = "Claude 3 Opus"),
    Claude3Sonnet UMETA(DisplayName = "Claude 3 Sonnet"),
    Claude3Haiku  UMETA(DisplayName = "Claude 3 Haiku"),
    GeminiPro     UMETA(DisplayName = "Gemini Pro"),
    GeminiUltra   UMETA(DisplayName = "Gemini Ultra"),
    Llama270B     UMETA(DisplayName = "Llama 2 70B"),
    Llama38B      UMETA(DisplayName = "Llama 3 8B"),
    Llama370B     UMETA(DisplayName = "Llama 3 70B"),
    Mistral7B     UMETA(DisplayName = "Mistral 7B"),
    Mixtral8x7B   UMETA(DisplayName = "Mixtral 8x7B"),
    CodeLlama     UMETA(DisplayName = "Code Llama"),
    StableDiffusion UMETA(DisplayName = "Stable Diffusion"),
    DALL_E3       UMETA(DisplayName = "DALL-E 3"),
    Whisper       UMETA(DisplayName = "Whisper"),
    Custom        UMETA(DisplayName = "Custom Model")
};

UENUM(BlueprintType)
enum class EModelStatus : uint8
{
    Available     UMETA(DisplayName = "Available"),
    Loading       UMETA(DisplayName = "Loading"),
    Active        UMETA(DisplayName = "Active"),
    Error         UMETA(DisplayName = "Error"),
    Offline       UMETA(DisplayName = "Offline")
};

USTRUCT(BlueprintType)
struct FIGEDModelConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ModelName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EModelType ModelType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString APIEndpoint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString APIKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxTokens;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Temperature;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Capabilities;

    FIGEDModelConfig()
    {
        ModelName = TEXT("");
        ModelType = EModelType::GPT4;
        APIEndpoint = TEXT("");
        APIKey = TEXT("");
        MaxTokens = 4096;
        Temperature = 0.7f;
        bEnabled = true;
        Description = TEXT("");
    }
};

USTRUCT(BlueprintType)
struct FIGEDModelPerformance
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ResponseTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TokensPerSecond;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Accuracy;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TotalRequests;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SuccessfulRequests;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime LastUsed;

    FIGEDModelPerformance()
    {
        ResponseTime = 0.0f;
        TokensPerSecond = 0;
        Accuracy = 0.0f;
        TotalRequests = 0;
        SuccessfulRequests = 0;
        LastUsed = FDateTime::Now();
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnModelResponse, const FString&, ModelName, const FString&, Response, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnModelStatusChanged, const FString&, ModelName, EModelStatus, NewStatus);

UCLASS(BlueprintType, Blueprintable)
class IGED_UE5_API UIGEDModelManager : public UObject
{
    GENERATED_BODY()

public:
    UIGEDModelManager();

    UFUNCTION(BlueprintCallable, Category = "IGED Model Manager")
    bool Initialize();

    UFUNCTION(BlueprintCallable, Category = "IGED Model Manager")
    void Shutdown();

    UFUNCTION(BlueprintCallable, Category = "IGED Model Manager")
    bool LoadModel(const FString& ModelName);

    UFUNCTION(BlueprintCallable, Category = "IGED Model Manager")
    bool UnloadModel(const FString& ModelName);

    UFUNCTION(BlueprintCallable, Category = "IGED Model Manager")
    bool SwitchActiveModel(const FString& ModelName);

    UFUNCTION(BlueprintCallable, Category = "IGED Model Manager")
    FString GetActiveModelName() const;

    UFUNCTION(BlueprintCallable, Category = "IGED Model Manager")
    EModelStatus GetModelStatus(const FString& ModelName) const;

    UFUNCTION(BlueprintCallable, Category = "IGED Model Manager")
    TArray<FString> GetAvailableModels() const;

    UFUNCTION(BlueprintCallable, Category = "IGED Model Manager")
    bool SendRequest(const FString& ModelName, const FString& Prompt, const TMap<FString, FString>& Parameters = TMap<FString, FString>());

    UFUNCTION(BlueprintCallable, Category = "IGED Model Manager")
    bool SendAsyncRequest(const FString& ModelName, const FString& Prompt, const TMap<FString, FString>& Parameters = TMap<FString, FString>());

    UFUNCTION(BlueprintCallable, Category = "IGED Model Manager")
    void AddModelConfig(const FIGEDModelConfig& Config);

    UFUNCTION(BlueprintCallable, Category = "IGED Model Manager")
    bool UpdateModelConfig(const FString& ModelName, const FIGEDModelConfig& NewConfig);

    UFUNCTION(BlueprintCallable, Category = "IGED Model Manager")
    FIGEDModelConfig GetModelConfig(const FString& ModelName) const;

    UFUNCTION(BlueprintCallable, Category = "IGED Model Manager")
    FIGEDModelPerformance GetModelPerformance(const FString& ModelName) const;

    UFUNCTION(BlueprintCallable, Category = "IGED Model Manager")
    void OptimizeModelPerformance(const FString& ModelName);

    UPROPERTY(BlueprintAssignable, Category = "IGED Model Manager")
    FOnModelResponse OnModelResponse;

    UPROPERTY(BlueprintAssignable, Category = "IGED Model Manager")
    FOnModelStatusChanged OnModelStatusChanged;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, FIGEDModelConfig> ModelConfigs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, FIGEDModelPerformance> ModelPerformance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    FString ActiveModelName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, EModelStatus> ModelStatuses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    int32 MaxConcurrentRequests;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    float RequestTimeout;

private:
    TMap<FString, TSharedPtr<IHttpRequest>> PendingRequests;
    TQueue<FString> RequestQueue;
    FCriticalSection RequestMutex;

    void ProcessRequestQueue();
    void HandleHttpResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, const FString& ModelName);
    void UpdateModelStatus(const FString& ModelName, EModelStatus NewStatus);
    FString FormatRequestPayload(const FString& ModelName, const FString& Prompt, const TMap<FString, FString>& Parameters);
    bool ValidateModelConfig(const FIGEDModelConfig& Config);
    void LoadDefaultModels();
    void InitializeModelPerformance();
};

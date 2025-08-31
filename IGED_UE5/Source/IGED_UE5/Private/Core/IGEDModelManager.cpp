#include "Core/IGEDModelManager.h"
#include "Engine/Engine.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

UIGEDModelManager::UIGEDModelManager()
{
    MaxConcurrentRequests = 10;
    RequestTimeout = 30.0f;
}

bool UIGEDModelManager::Initialize()
{
    UE_LOG(LogTemp, Warning, TEXT("IGED Model Manager: Initializing..."));
    
    LoadDefaultModels();
    InitializeModelPerformance();
    
    UE_LOG(LogTemp, Warning, TEXT("IGED Model Manager: Initialized with %d models"), ModelConfigs.Num());
    return true;
}

void UIGEDModelManager::Shutdown()
{
    UE_LOG(LogTemp, Warning, TEXT("IGED Model Manager: Shutting down..."));
    
    // Cancel all pending requests
    for (auto& RequestPair : PendingRequests)
    {
        if (RequestPair.Value.IsValid())
        {
            RequestPair.Value->CancelRequest();
        }
    }
    PendingRequests.Empty();
    
    // Clear request queue
    while (!RequestQueue.IsEmpty())
    {
        FString Dummy;
        RequestQueue.Dequeue(Dummy);
    }
    
    ModelConfigs.Empty();
    ModelPerformance.Empty();
    ModelStatuses.Empty();
}

bool UIGEDModelManager::LoadModel(const FString& ModelName)
{
    if (!ModelConfigs.Contains(ModelName))
    {
        UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: Model %s not found in configs"), *ModelName);
        return false;
    }
    
    UpdateModelStatus(ModelName, EModelStatus::Loading);
    
    // Simulate model loading (in real implementation, this would load the actual model)
    FTimerHandle TimerHandle;
    GEngine->GetWorldFromContextObject(this)->GetTimerManager().SetTimer(TimerHandle, [this, ModelName]()
    {
        UpdateModelStatus(ModelName, EModelStatus::Available);
        UE_LOG(LogTemp, Warning, TEXT("IGED Model Manager: Model %s loaded successfully"), *ModelName);
    }, 2.0f, false);
    
    return true;
}

bool UIGEDModelManager::UnloadModel(const FString& ModelName)
{
    if (!ModelConfigs.Contains(ModelName))
    {
        UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: Model %s not found"), *ModelName);
        return false;
    }
    
    UpdateModelStatus(ModelName, EModelStatus::Offline);
    UE_LOG(LogTemp, Warning, TEXT("IGED Model Manager: Model %s unloaded"), *ModelName);
    return true;
}

bool UIGEDModelManager::SwitchActiveModel(const FString& ModelName)
{
    if (!ModelConfigs.Contains(ModelName))
    {
        UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: Model %s not found"), *ModelName);
        return false;
    }
    
    EModelStatus Status = GetModelStatus(ModelName);
    if (Status != EModelStatus::Available && Status != EModelStatus::Active)
    {
        UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: Model %s is not available (Status: %d)"), *ModelName, (int32)Status);
        return false;
    }
    
    ActiveModelName = ModelName;
    UpdateModelStatus(ModelName, EModelStatus::Active);
    
    UE_LOG(LogTemp, Warning, TEXT("IGED Model Manager: Switched to model %s"), *ModelName);
    return true;
}

FString UIGEDModelManager::GetActiveModelName() const
{
    return ActiveModelName;
}

EModelStatus UIGEDModelManager::GetModelStatus(const FString& ModelName) const
{
    if (const EModelStatus* Status = ModelStatuses.Find(ModelName))
    {
        return *Status;
    }
    return EModelStatus::Offline;
}

TArray<FString> UIGEDModelManager::GetAvailableModels() const
{
    TArray<FString> AvailableModels;
    for (const auto& ConfigPair : ModelConfigs)
    {
        if (ConfigPair.Value.bEnabled)
        {
            AvailableModels.Add(ConfigPair.Key);
        }
    }
    return AvailableModels;
}

bool UIGEDModelManager::SendRequest(const FString& ModelName, const FString& Prompt, const TMap<FString, FString>& Parameters)
{
    if (!ModelConfigs.Contains(ModelName))
    {
        UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: Model %s not found"), *ModelName);
        return false;
    }
    
    const FIGEDModelConfig& Config = ModelConfigs[ModelName];
    if (!Config.bEnabled)
    {
        UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: Model %s is disabled"), *ModelName);
        return false;
    }
    
    return SendAsyncRequest(ModelName, Prompt, Parameters);
}

bool UIGEDModelManager::SendAsyncRequest(const FString& ModelName, const FString& Prompt, const TMap<FString, FString>& Parameters)
{
    if (!ModelConfigs.Contains(ModelName))
    {
        UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: Model %s not found"), *ModelName);
        return false;
    }
    
    const FIGEDModelConfig& Config = ModelConfigs[ModelName];
    
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(Config.APIEndpoint);
    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Config.APIKey));
    HttpRequest->SetTimeout(RequestTimeout);
    
    FString RequestPayload = FormatRequestPayload(ModelName, Prompt, Parameters);
    HttpRequest->SetContentAsString(RequestPayload);
    
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UIGEDModelManager::HandleHttpResponse, ModelName);
    
    PendingRequests.Add(ModelName, HttpRequest);
    
    if (!HttpRequest->ProcessRequest())
    {
        UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: Failed to process request for model %s"), *ModelName);
        PendingRequests.Remove(ModelName);
        return false;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("IGED Model Manager: Sent request to model %s"), *ModelName);
    return true;
}

void UIGEDModelManager::AddModelConfig(const FIGEDModelConfig& Config)
{
    if (ValidateModelConfig(Config))
    {
        ModelConfigs.Add(Config.ModelName, Config);
        ModelStatuses.Add(Config.ModelName, EModelStatus::Offline);
        UE_LOG(LogTemp, Warning, TEXT("IGED Model Manager: Added model config for %s"), *Config.ModelName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: Invalid model config for %s"), *Config.ModelName);
    }
}

bool UIGEDModelManager::UpdateModelConfig(const FString& ModelName, const FIGEDModelConfig& NewConfig)
{
    if (!ModelConfigs.Contains(ModelName))
    {
        UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: Model %s not found for update"), *ModelName);
        return false;
    }
    
    if (ValidateModelConfig(NewConfig))
    {
        ModelConfigs[ModelName] = NewConfig;
        UE_LOG(LogTemp, Warning, TEXT("IGED Model Manager: Updated model config for %s"), *ModelName);
        return true;
    }
    
    UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: Invalid model config for %s"), *ModelName);
    return false;
}

FIGEDModelConfig UIGEDModelManager::GetModelConfig(const FString& ModelName) const
{
    if (const FIGEDModelConfig* Config = ModelConfigs.Find(ModelName))
    {
        return *Config;
    }
    return FIGEDModelConfig();
}

FIGEDModelPerformance UIGEDModelManager::GetModelPerformance(const FString& ModelName) const
{
    if (const FIGEDModelPerformance* Performance = ModelPerformance.Find(ModelName))
    {
        return *Performance;
    }
    return FIGEDModelPerformance();
}

void UIGEDModelManager::OptimizeModelPerformance(const FString& ModelName)
{
    if (!ModelPerformance.Contains(ModelName))
    {
        UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: No performance data for model %s"), *ModelName);
        return;
    }
    
    FIGEDModelPerformance& Performance = ModelPerformance[ModelName];
    
    // Simulate optimization (in real implementation, this would apply actual optimizations)
    Performance.ResponseTime *= 0.9f; // 10% improvement
    Performance.TokensPerSecond = FMath::Max(Performance.TokensPerSecond + 100, 0);
    
    UE_LOG(LogTemp, Warning, TEXT("IGED Model Manager: Optimized performance for model %s"), *ModelName);
}

void UIGEDModelManager::ProcessRequestQueue()
{
    // Process queued requests when resources are available
    FScopeLock Lock(&RequestMutex);
    
    while (!RequestQueue.IsEmpty() && PendingRequests.Num() < MaxConcurrentRequests)
    {
        FString ModelName;
        if (RequestQueue.Dequeue(ModelName))
        {
            // Process the request
            UE_LOG(LogTemp, Warning, TEXT("IGED Model Manager: Processing queued request for %s"), *ModelName);
        }
    }
}

void UIGEDModelManager::HandleHttpResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, const FString& ModelName)
{
    PendingRequests.Remove(ModelName);
    
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: Request failed for model %s"), *ModelName);
        OnModelResponse.Broadcast(ModelName, TEXT(""), false);
        UpdateModelStatus(ModelName, EModelStatus::Error);
        return;
    }
    
    FString ResponseContent = Response->GetContentAsString();
    int32 ResponseCode = Response->GetResponseCode();
    
    if (ResponseCode >= 200 && ResponseCode < 300)
    {
        UE_LOG(LogTemp, Warning, TEXT("IGED Model Manager: Received response from model %s"), *ModelName);
        OnModelResponse.Broadcast(ModelName, ResponseContent, true);
        UpdateModelStatus(ModelName, EModelStatus::Available);
        
        // Update performance metrics
        if (FIGEDModelPerformance* Performance = ModelPerformance.Find(ModelName))
        {
            Performance->TotalRequests++;
            Performance->SuccessfulRequests++;
            Performance->LastUsed = FDateTime::Now();
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: HTTP error %d for model %s"), ResponseCode, *ModelName);
        OnModelResponse.Broadcast(ModelName, ResponseContent, false);
        UpdateModelStatus(ModelName, EModelStatus::Error);
    }
}

void UIGEDModelManager::UpdateModelStatus(const FString& ModelName, EModelStatus NewStatus)
{
    ModelStatuses[ModelName] = NewStatus;
    OnModelStatusChanged.Broadcast(ModelName, NewStatus);
}

FString UIGEDModelManager::FormatRequestPayload(const FString& ModelName, const FString& Prompt, const TMap<FString, FString>& Parameters)
{
    const FIGEDModelConfig& Config = ModelConfigs[ModelName];
    
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField(TEXT("model"), ModelName);
    JsonObject->SetStringField(TEXT("prompt"), Prompt);
    JsonObject->SetNumberField(TEXT("max_tokens"), Config.MaxTokens);
    JsonObject->SetNumberField(TEXT("temperature"), Config.Temperature);
    
    // Add custom parameters
    for (const auto& ParamPair : Parameters)
    {
        JsonObject->SetStringField(ParamPair.Key, ParamPair.Value);
    }
    
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    
    return OutputString;
}

bool UIGEDModelManager::ValidateModelConfig(const FIGEDModelConfig& Config)
{
    if (Config.ModelName.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: Model name cannot be empty"));
        return false;
    }
    
    if (Config.APIEndpoint.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: API endpoint cannot be empty for model %s"), *Config.ModelName);
        return false;
    }
    
    if (Config.APIKey.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: API key cannot be empty for model %s"), *Config.ModelName);
        return false;
    }
    
    if (Config.MaxTokens <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: Max tokens must be positive for model %s"), *Config.ModelName);
        return false;
    }
    
    if (Config.Temperature < 0.0f || Config.Temperature > 2.0f)
    {
        UE_LOG(LogTemp, Error, TEXT("IGED Model Manager: Temperature must be between 0.0 and 2.0 for model %s"), *Config.ModelName);
        return false;
    }
    
    return true;
}

void UIGEDModelManager::LoadDefaultModels()
{
    // GPT-4
    FIGEDModelConfig GPT4Config;
    GPT4Config.ModelName = TEXT("gpt-4");
    GPT4Config.ModelType = EModelType::GPT4;
    GPT4Config.APIEndpoint = TEXT("https://api.openai.com/v1/chat/completions");
    GPT4Config.APIKey = TEXT("your-openai-api-key");
    GPT4Config.MaxTokens = 8192;
    GPT4Config.Temperature = 0.7f;
    GPT4Config.Description = TEXT("Most capable GPT-4 model for complex reasoning tasks");
    GPT4Config.Capabilities = {TEXT("text-generation"), TEXT("reasoning"), TEXT("code-generation")};
    AddModelConfig(GPT4Config);
    
    // GPT-4 Turbo
    FIGEDModelConfig GPT4TurboConfig;
    GPT4TurboConfig.ModelName = TEXT("gpt-4-turbo");
    GPT4TurboConfig.ModelType = EModelType::GPT4Turbo;
    GPT4TurboConfig.APIEndpoint = TEXT("https://api.openai.com/v1/chat/completions");
    GPT4TurboConfig.APIKey = TEXT("your-openai-api-key");
    GPT4TurboConfig.MaxTokens = 128000;
    GPT4TurboConfig.Temperature = 0.7f;
    GPT4TurboConfig.Description = TEXT("Faster and more efficient GPT-4 model");
    GPT4TurboConfig.Capabilities = {TEXT("text-generation"), TEXT("reasoning"), TEXT("code-generation"), TEXT("large-context")};
    AddModelConfig(GPT4TurboConfig);
    
    // Claude 3 Opus
    FIGEDModelConfig Claude3OpusConfig;
    Claude3OpusConfig.ModelName = TEXT("claude-3-opus-20240229");
    Claude3OpusConfig.ModelType = EModelType::Claude3Opus;
    Claude3OpusConfig.APIEndpoint = TEXT("https://api.anthropic.com/v1/messages");
    Claude3OpusConfig.APIKey = TEXT("your-anthropic-api-key");
    Claude3OpusConfig.MaxTokens = 4096;
    Claude3OpusConfig.Temperature = 0.7f;
    Claude3OpusConfig.Description = TEXT("Most powerful Claude 3 model for complex tasks");
    Claude3OpusConfig.Capabilities = {TEXT("text-generation"), TEXT("reasoning"), TEXT("analysis")};
    AddModelConfig(Claude3OpusConfig);
    
    // Claude 3 Sonnet
    FIGEDModelConfig Claude3SonnetConfig;
    Claude3SonnetConfig.ModelName = TEXT("claude-3-sonnet-20240229");
    Claude3SonnetConfig.ModelType = EModelType::Claude3Sonnet;
    Claude3SonnetConfig.APIEndpoint = TEXT("https://api.anthropic.com/v1/messages");
    Claude3SonnetConfig.APIKey = TEXT("your-anthropic-api-key");
    Claude3SonnetConfig.MaxTokens = 4096;
    Claude3SonnetConfig.Temperature = 0.7f;
    Claude3SonnetConfig.Description = TEXT("Balanced Claude 3 model for most tasks");
    Claude3SonnetConfig.Capabilities = {TEXT("text-generation"), TEXT("reasoning"), TEXT("analysis")};
    AddModelConfig(Claude3SonnetConfig);
    
    // Claude 3 Haiku
    FIGEDModelConfig Claude3HaikuConfig;
    Claude3HaikuConfig.ModelName = TEXT("claude-3-haiku-20240307");
    Claude3HaikuConfig.ModelType = EModelType::Claude3Haiku;
    Claude3HaikuConfig.APIEndpoint = TEXT("https://api.anthropic.com/v1/messages");
    Claude3HaikuConfig.APIKey = TEXT("your-anthropic-api-key");
    Claude3HaikuConfig.MaxTokens = 4096;
    Claude3HaikuConfig.Temperature = 0.7f;
    Claude3HaikuConfig.Description = TEXT("Fast and efficient Claude 3 model");
    Claude3HaikuConfig.Capabilities = {TEXT("text-generation"), TEXT("quick-responses")};
    AddModelConfig(Claude3HaikuConfig);
    
    // Gemini Pro
    FIGEDModelConfig GeminiProConfig;
    GeminiProConfig.ModelName = TEXT("gemini-pro");
    GeminiProConfig.ModelType = EModelType::GeminiPro;
    GeminiProConfig.APIEndpoint = TEXT("https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent");
    GeminiProConfig.APIKey = TEXT("your-google-api-key");
    GeminiProConfig.MaxTokens = 8192;
    GeminiProConfig.Temperature = 0.7f;
    GeminiProConfig.Description = TEXT("Google's Gemini Pro model for text generation");
    GeminiProConfig.Capabilities = {TEXT("text-generation"), TEXT("multimodal")};
    AddModelConfig(GeminiProConfig);
    
    // Llama 3 70B
    FIGEDModelConfig Llama370BConfig;
    Llama370BConfig.ModelName = TEXT("llama-3-70b");
    Llama370BConfig.ModelType = EModelType::Llama370B;
    Llama370BConfig.APIEndpoint = TEXT("https://api.replicate.com/v1/predictions");
    Llama370BConfig.APIKey = TEXT("your-replicate-api-key");
    Llama370BConfig.MaxTokens = 4096;
    Llama370BConfig.Temperature = 0.7f;
    Llama370BConfig.Description = TEXT("Meta's Llama 3 70B model for advanced reasoning");
    Llama370BConfig.Capabilities = {TEXT("text-generation"), TEXT("reasoning"), TEXT("open-source")};
    AddModelConfig(Llama370BConfig);
    
    // Mistral 7B
    FIGEDModelConfig Mistral7BConfig;
    Mistral7BConfig.ModelName = TEXT("mistral-7b");
    Mistral7BConfig.ModelType = EModelType::Mistral7B;
    Mistral7BConfig.APIEndpoint = TEXT("https://api.mistral.ai/v1/chat/completions");
    Mistral7BConfig.APIKey = TEXT("your-mistral-api-key");
    Mistral7BConfig.MaxTokens = 4096;
    Mistral7BConfig.Temperature = 0.7f;
    Mistral7BConfig.Description = TEXT("Mistral 7B model for efficient text generation");
    Mistral7BConfig.Capabilities = {TEXT("text-generation"), TEXT("efficient")};
    AddModelConfig(Mistral7BConfig);
    
    // Mixtral 8x7B
    FIGEDModelConfig Mixtral8x7BConfig;
    Mixtral8x7BConfig.ModelName = TEXT("mixtral-8x7b");
    Mixtral8x7BConfig.ModelType = EModelType::Mixtral8x7B;
    Mixtral8x7BConfig.APIEndpoint = TEXT("https://api.mistral.ai/v1/chat/completions");
    Mixtral8x7BConfig.APIKey = TEXT("your-mistral-api-key");
    Mixtral8x7BConfig.MaxTokens = 4096;
    Mixtral8x7BConfig.Temperature = 0.7f;
    Mixtral8x7BConfig.Description = TEXT("Mixtral 8x7B mixture of experts model");
    Mixtral8x7BConfig.Capabilities = {TEXT("text-generation"), TEXT("mixture-of-experts")};
    AddModelConfig(Mixtral8x7BConfig);
    
    // Code Llama
    FIGEDModelConfig CodeLlamaConfig;
    CodeLlamaConfig.ModelName = TEXT("codellama-34b");
    CodeLlamaConfig.ModelType = EModelType::CodeLlama;
    CodeLlamaConfig.APIEndpoint = TEXT("https://api.replicate.com/v1/predictions");
    CodeLlamaConfig.APIKey = TEXT("your-replicate-api-key");
    CodeLlamaConfig.MaxTokens = 4096;
    CodeLlamaConfig.Temperature = 0.1f;
    CodeLlamaConfig.Description = TEXT("Specialized Llama model for code generation");
    CodeLlamaConfig.Capabilities = {TEXT("code-generation"), TEXT("programming"), TEXT("debugging")};
    AddModelConfig(CodeLlamaConfig);
    
    // DALL-E 3
    FIGEDModelConfig DALLE3Config;
    DALLE3Config.ModelName = TEXT("dall-e-3");
    DALLE3Config.ModelType = EModelType::DALL_E3;
    DALLE3Config.APIEndpoint = TEXT("https://api.openai.com/v1/images/generations");
    DALLE3Config.APIKey = TEXT("your-openai-api-key");
    DALLE3Config.MaxTokens = 1;
    DALLE3Config.Temperature = 0.7f;
    DALLE3Config.Description = TEXT("OpenAI's DALL-E 3 for image generation");
    DALLE3Config.Capabilities = {TEXT("image-generation"), TEXT("art"), TEXT("visual")};
    AddModelConfig(DALLE3Config);
    
    // Stable Diffusion
    FIGEDModelConfig StableDiffusionConfig;
    StableDiffusionConfig.ModelName = TEXT("stable-diffusion-xl");
    StableDiffusionConfig.ModelType = EModelType::StableDiffusion;
    StableDiffusionConfig.APIEndpoint = TEXT("https://api.replicate.com/v1/predictions");
    StableDiffusionConfig.APIKey = TEXT("your-replicate-api-key");
    StableDiffusionConfig.MaxTokens = 1;
    StableDiffusionConfig.Temperature = 0.7f;
    StableDiffusionConfig.Description = TEXT("Stable Diffusion XL for high-quality image generation");
    StableDiffusionConfig.Capabilities = {TEXT("image-generation"), TEXT("open-source"), TEXT("customizable")};
    AddModelConfig(StableDiffusionConfig);
    
    // Whisper
    FIGEDModelConfig WhisperConfig;
    WhisperConfig.ModelName = TEXT("whisper-1");
    WhisperConfig.ModelType = EModelType::Whisper;
    WhisperConfig.APIEndpoint = TEXT("https://api.openai.com/v1/audio/transcriptions");
    WhisperConfig.APIKey = TEXT("your-openai-api-key");
    WhisperConfig.MaxTokens = 4096;
    WhisperConfig.Temperature = 0.0f;
    WhisperConfig.Description = TEXT("OpenAI's Whisper for speech-to-text transcription");
    WhisperConfig.Capabilities = {TEXT("speech-to-text"), TEXT("transcription"), TEXT("multilingual")};
    AddModelConfig(WhisperConfig);
}

void UIGEDModelManager::InitializeModelPerformance()
{
    for (const auto& ConfigPair : ModelConfigs)
    {
        FIGEDModelPerformance Performance;
        Performance.ResponseTime = 1.0f; // Default 1 second
        Performance.TokensPerSecond = 1000; // Default 1000 tokens/sec
        Performance.Accuracy = 0.95f; // Default 95% accuracy
        Performance.TotalRequests = 0;
        Performance.SuccessfulRequests = 0;
        Performance.LastUsed = FDateTime::Now();
        
        ModelPerformance.Add(ConfigPair.Key, Performance);
    }
}

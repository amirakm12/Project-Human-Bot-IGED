// IGED Advanced Networking System - Enhanced Networking and API Management
// Copyright (c) 2024 Project Human Bot

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "IGEDAdvancedNetworking.generated.h"

// Network request structure
USTRUCT(BlueprintType)
struct FNetworkRequest
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    FString RequestID;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    FString URL;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    FString Method;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    FString Headers;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    FString Body;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    float Timeout;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    int32 RetryCount;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    bool bIsAsync;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    FDateTime CreatedTime;

    FNetworkRequest()
        : Timeout(30.0f)
        , RetryCount(3)
        , bIsAsync(true)
    {}
};

// Network response structure
USTRUCT(BlueprintType)
struct FNetworkResponse
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    FString RequestID;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    int32 StatusCode;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    FString StatusText;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    FString Headers;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    FString Body;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    float ResponseTime;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    bool bIsSuccess;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    FString ErrorMessage;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    FDateTime ReceivedTime;

    FNetworkResponse()
        : StatusCode(0)
        , ResponseTime(0.0f)
        , bIsSuccess(false)
    {}
};

// API endpoint structure
USTRUCT(BlueprintType)
struct FAPIEndpoint
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    FString EndpointID;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    FString EndpointName;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    FString BaseURL;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    FString Path;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    FString Method;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    TMap<FString, FString> DefaultHeaders;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    float Timeout;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    int32 RetryCount;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    bool bRequiresAuth;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    bool bIsActive;

    FAPIEndpoint()
        : Timeout(30.0f)
        , RetryCount(3)
        , bRequiresAuth(false)
        , bIsActive(true)
    {}
};

// Network statistics structure
USTRUCT(BlueprintType)
struct FNetworkStatistics
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    int32 TotalRequests;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    int32 SuccessfulRequests;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    int32 FailedRequests;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    float SuccessRate;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    float AverageResponseTime;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    float MinResponseTime;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    float MaxResponseTime;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    int32 ActiveConnections;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    float BandwidthUsage;

    FNetworkStatistics()
        : TotalRequests(0)
        , SuccessfulRequests(0)
        , FailedRequests(0)
        , SuccessRate(0.0f)
        , AverageResponseTime(0.0f)
        , MinResponseTime(0.0f)
        , MaxResponseTime(0.0f)
        , ActiveConnections(0)
        , BandwidthUsage(0.0f)
    {}
};

// Network configuration structure
USTRUCT(BlueprintType)
struct FNetworkConfiguration
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    bool bEnableCompression;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    bool bEnableCaching;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    bool bEnableRetry;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    bool bEnableLoadBalancing;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    bool bEnableCircuitBreaker;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    float DefaultTimeout;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    int32 MaxRetryCount;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    int32 MaxConcurrentRequests;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Networking")
    float CircuitBreakerThreshold;

    FNetworkConfiguration()
        : bEnableCompression(true)
        , bEnableCaching(true)
        , bEnableRetry(true)
        , bEnableLoadBalancing(true)
        , bEnableCircuitBreaker(true)
        , DefaultTimeout(30.0f)
        , MaxRetryCount(3)
        , MaxConcurrentRequests(10)
        , CircuitBreakerThreshold(0.5f)
    {}
};

// HTTP method enum
UENUM(BlueprintType)
enum class EHTTPMethod : uint8
{
    GET UMETA(DisplayName = "GET"),
    POST UMETA(DisplayName = "POST"),
    PUT UMETA(DisplayName = "PUT"),
    DELETE UMETA(DisplayName = "DELETE"),
    PATCH UMETA(DisplayName = "PATCH"),
    HEAD UMETA(DisplayName = "HEAD"),
    OPTIONS UMETA(DisplayName = "OPTIONS")
};

// Network status enum
UENUM(BlueprintType)
enum class ENetworkStatus : uint8
{
    Disconnected UMETA(DisplayName = "Disconnected"),
    Connecting UMETA(DisplayName = "Connecting"),
    Connected UMETA(DisplayName = "Connected"),
    Reconnecting UMETA(DisplayName = "Reconnecting"),
    Error UMETA(DisplayName = "Error")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNetworkRequestCompleted, const FNetworkResponse&, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNetworkRequestFailed, const FNetworkResponse&, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNetworkStatusChanged, ENetworkStatus, NewStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNetworkStatisticsUpdated, const FNetworkStatistics&, Statistics);

UCLASS(Blueprintable)
class IGED_UE5_API UIGEDAdvancedNetworking : public UObject
{
    GENERATED_BODY()

public:
    UIGEDAdvancedNetworking();

    // Initialization
    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void Initialize(const FNetworkConfiguration& Config);

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void Shutdown();

    // Request management
    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    FString SendRequest(const FNetworkRequest& Request);

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    FString SendRequestAsync(const FNetworkRequest& Request);

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    FNetworkResponse SendRequestSync(const FNetworkRequest& Request);

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void CancelRequest(const FString& RequestID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void CancelAllRequests();

    // API endpoint management
    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    FString RegisterEndpoint(const FAPIEndpoint& Endpoint);

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    bool UnregisterEndpoint(const FString& EndpointID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    FAPIEndpoint GetEndpoint(const FString& EndpointID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    TArray<FAPIEndpoint> GetAllEndpoints();

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    FString CallEndpoint(const FString& EndpointID, const FString& Parameters = TEXT(""));

    // Connection management
    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void Connect();

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void Disconnect();

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void Reconnect();

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    ENetworkStatus GetNetworkStatus() const;

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    bool IsConnected() const;

    // Performance optimization
    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void EnableCompression(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void EnableCaching(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void EnableLoadBalancing(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void EnableCircuitBreaker(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void OptimizeNetworkPerformance();

    // Statistics and monitoring
    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    FNetworkStatistics GetStatistics();

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void StartMonitoring();

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void StopMonitoring();

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void UpdateStatistics();

    // Advanced features
    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void EnableRetryMechanism(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void SetRetryCount(int32 Count);

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void SetTimeout(float Timeout);

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void SetMaxConcurrentRequests(int32 MaxRequests);

    // Configuration
    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    void SetConfiguration(const FNetworkConfiguration& Config);

    UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
    FNetworkConfiguration GetConfiguration() const;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "IGED|Networking|Events")
    FOnNetworkRequestCompleted OnNetworkRequestCompleted;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Networking|Events")
    FOnNetworkRequestFailed OnNetworkRequestFailed;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Networking|Events")
    FOnNetworkStatusChanged OnNetworkStatusChanged;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Networking|Events")
    FOnNetworkStatisticsUpdated OnNetworkStatisticsUpdated;

protected:
    UPROPERTY()
    bool bIsInitialized;

    UPROPERTY()
    bool bIsMonitoring;

    UPROPERTY()
    FNetworkConfiguration Configuration;

    UPROPERTY()
    ENetworkStatus NetworkStatus;

    UPROPERTY()
    TMap<FString, FNetworkRequest> ActiveRequests;

    UPROPERTY()
    TMap<FString, FAPIEndpoint> Endpoints;

    UPROPERTY()
    FNetworkStatistics Statistics;

    UPROPERTY()
    TArray<FString> RequestQueue;

    UPROPERTY()
    int32 ActiveRequestCount;

private:
    void ProcessRequestQueue();
    void HandleRequestCompleted(const FString& RequestID, const FNetworkResponse& Response);
    void HandleRequestFailed(const FString& RequestID, const FNetworkResponse& Response);
    void UpdateNetworkStatus(ENetworkStatus NewStatus);
    
    // Request processing
    void ProcessRequest(const FNetworkRequest& Request);
    void ProcessRequestAsync(const FNetworkRequest& Request);
    FNetworkResponse ProcessRequestSync(const FNetworkRequest& Request);
    
    // Performance optimization
    void OptimizeRequestProcessing();
    void OptimizeConnectionPool();
    void OptimizeCaching();
    void OptimizeLoadBalancing();
    
    // Circuit breaker
    void CheckCircuitBreaker();
    void OpenCircuitBreaker();
    void CloseCircuitBreaker();
    bool IsCircuitBreakerOpen();
    
    // Utility functions
    FString GenerateRequestID();
    FNetworkResponse CreateErrorResponse(const FString& RequestID, const FString& ErrorMessage);
    void LogNetworkEvent(const FString& Event, const FString& Details);
    void UpdateRequestStatistics(const FNetworkResponse& Response);
    
    // Timer handles
    FTimerHandle RequestProcessingTimer;
    FTimerHandle StatisticsTimer;
    FTimerHandle CircuitBreakerTimer;
    FTimerHandle MonitoringTimer;
};

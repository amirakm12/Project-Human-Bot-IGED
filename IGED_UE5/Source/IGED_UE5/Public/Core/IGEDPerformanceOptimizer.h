// IGED Performance Optimizer - Advanced Performance Monitoring and Optimization
// Copyright (c) 2024 Project Human Bot

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Engine.h"
#include "HAL/PlatformFilemanager.h"
#include "IGEDPerformanceOptimizer.generated.h"

// Performance metrics structure
USTRUCT(BlueprintType)
struct FPerformanceMetrics
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Performance")
    float CPUTime;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Performance")
    float MemoryUsage;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Performance")
    float GPUTime;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Performance")
    float FrameTime;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Performance")
    float NetworkLatency;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Performance")
    int32 ActiveThreads;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Performance")
    float CacheHitRate;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Performance")
    float AIProcessingTime;

    FPerformanceMetrics()
        : CPUTime(0.0f)
        , MemoryUsage(0.0f)
        , GPUTime(0.0f)
        , FrameTime(0.0f)
        , NetworkLatency(0.0f)
        , ActiveThreads(0)
        , CacheHitRate(0.0f)
        , AIProcessingTime(0.0f)
    {}
};

// Optimization level enum
UENUM(BlueprintType)
enum class EOptimizationLevel : uint8
{
    Conservative UMETA(DisplayName = "Conservative"),
    Balanced UMETA(DisplayName = "Balanced"),
    Aggressive UMETA(DisplayName = "Aggressive"),
    Maximum UMETA(DisplayName = "Maximum Performance")
};

// Performance profile structure
USTRUCT(BlueprintType)
struct FPerformanceProfile
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Performance")
    FString ProfileName;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Performance")
    EOptimizationLevel OptimizationLevel;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Performance")
    float TargetFrameRate;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Performance")
    float MaxMemoryUsage;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Performance")
    bool bEnableMultiThreading;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Performance")
    bool bEnableAdvancedCaching;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Performance")
    bool bEnableGPUAcceleration;

    FPerformanceProfile()
        : OptimizationLevel(EOptimizationLevel::Balanced)
        , TargetFrameRate(60.0f)
        , MaxMemoryUsage(2048.0f)
        , bEnableMultiThreading(true)
        , bEnableAdvancedCaching(true)
        , bEnableGPUAcceleration(true)
    {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerformanceOptimized, const FPerformanceMetrics&, NewMetrics);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOptimizationProfileChanged, const FPerformanceProfile&, NewProfile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerformanceAlert, const FString&, AlertMessage);

UCLASS(Blueprintable)
class IGED_UE5_API UIGEDPerformanceOptimizer : public UObject
{
    GENERATED_BODY()

public:
    UIGEDPerformanceOptimizer();

    // Initialization
    UFUNCTION(BlueprintCallable, Category = "IGED|Performance")
    void Initialize();

    UFUNCTION(BlueprintCallable, Category = "IGED|Performance")
    void Shutdown();

    // Performance monitoring
    UFUNCTION(BlueprintCallable, Category = "IGED|Performance")
    FPerformanceMetrics GetCurrentMetrics();

    UFUNCTION(BlueprintCallable, Category = "IGED|Performance")
    void StartPerformanceMonitoring();

    UFUNCTION(BlueprintCallable, Category = "IGED|Performance")
    void StopPerformanceMonitoring();

    // Optimization
    UFUNCTION(BlueprintCallable, Category = "IGED|Performance")
    void OptimizePerformance(EOptimizationLevel Level);

    UFUNCTION(BlueprintCallable, Category = "IGED|Performance")
    void SetPerformanceProfile(const FPerformanceProfile& Profile);

    UFUNCTION(BlueprintCallable, Category = "IGED|Performance")
    FPerformanceProfile GetCurrentProfile() const;

    // Advanced features
    UFUNCTION(BlueprintCallable, Category = "IGED|Performance")
    void EnableAdvancedCaching(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "IGED|Performance")
    void EnableMultiThreading(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "IGED|Performance")
    void EnableGPUAcceleration(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "IGED|Performance")
    void OptimizeMemoryUsage();

    UFUNCTION(BlueprintCallable, Category = "IGED|Performance")
    void OptimizeNetworkPerformance();

    UFUNCTION(BlueprintCallable, Category = "IGED|Performance")
    void OptimizeAIPerformance();

    // Analytics
    UFUNCTION(BlueprintCallable, Category = "IGED|Performance")
    TArray<FPerformanceMetrics> GetPerformanceHistory();

    UFUNCTION(BlueprintCallable, Category = "IGED|Performance")
    FString GeneratePerformanceReport();

    UFUNCTION(BlueprintCallable, Category = "IGED|Performance")
    float GetPerformanceScore();

    // Events
    UPROPERTY(BlueprintAssignable, Category = "IGED|Performance|Events")
    FOnPerformanceOptimized OnPerformanceOptimized;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Performance|Events")
    FOnOptimizationProfileChanged OnOptimizationProfileChanged;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Performance|Events")
    FOnPerformanceAlert OnPerformanceAlert;

protected:
    UPROPERTY()
    bool bIsInitialized;

    UPROPERTY()
    bool bIsMonitoring;

    UPROPERTY()
    FPerformanceProfile CurrentProfile;

    UPROPERTY()
    TArray<FPerformanceMetrics> PerformanceHistory;

    UPROPERTY()
    float LastOptimizationTime;

    UPROPERTY()
    int32 OptimizationCount;

private:
    void UpdatePerformanceMetrics();
    void ApplyOptimizationSettings(EOptimizationLevel Level);
    void CheckPerformanceThresholds();
    void LogPerformanceData();
    
    // Advanced optimization methods
    void OptimizeRenderingPipeline();
    void OptimizeMemoryAllocation();
    void OptimizeThreadPool();
    void OptimizeCacheStrategy();
    void OptimizeNetworkConnections();
    void OptimizeAIProcessing();
    
    // Performance analysis
    float CalculatePerformanceScore() const;
    bool IsPerformanceDegraded() const;
    void GenerateOptimizationRecommendations();
    
    // Timer handles for monitoring
    FTimerHandle PerformanceMonitoringTimer;
    FTimerHandle OptimizationTimer;
};

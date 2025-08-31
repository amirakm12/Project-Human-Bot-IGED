// IGED Advanced Caching System - Intelligent Caching and Memory Optimization
// Copyright (c) 2024 Project Human Bot

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IGEDAdvancedCaching.generated.h"

// Cache entry structure
USTRUCT(BlueprintType)
struct FCacheEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    FString Key;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    FString Data;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    FDateTime CreatedTime;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    FDateTime LastAccessedTime;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    int32 AccessCount;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    float Priority;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    int32 Size;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    bool bIsPersistent;

    FCacheEntry()
        : AccessCount(0)
        , Priority(1.0f)
        , Size(0)
        , bIsPersistent(false)
    {}
};

// Cache statistics structure
USTRUCT(BlueprintType)
struct FCacheStatistics
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    int32 TotalEntries;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    int32 TotalSize;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    float HitRate;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    float MissRate;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    int32 Evictions;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    float AverageAccessTime;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    int32 MaxSize;

    FCacheStatistics()
        : TotalEntries(0)
        , TotalSize(0)
        , HitRate(0.0f)
        , MissRate(0.0f)
        , Evictions(0)
        , AverageAccessTime(0.0f)
        , MaxSize(0)
    {}
};

// Cache policy enum
UENUM(BlueprintType)
enum class ECachePolicy : uint8
{
    LRU UMETA(DisplayName = "Least Recently Used"),
    LFU UMETA(DisplayName = "Least Frequently Used"),
    FIFO UMETA(DisplayName = "First In First Out"),
    LIFO UMETA(DisplayName = "Last In First Out"),
    Priority UMETA(DisplayName = "Priority Based"),
    Adaptive UMETA(DisplayName = "Adaptive")
};

// Cache configuration structure
USTRUCT(BlueprintType)
struct FCacheConfiguration
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    int32 MaxSize;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    int32 MaxEntries;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    ECachePolicy Policy;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    float TTL; // Time to live in seconds

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    bool bEnableCompression;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    bool bEnablePersistence;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Caching")
    bool bEnablePreloading;

    FCacheConfiguration()
        : MaxSize(1024 * 1024 * 100) // 100MB default
        , MaxEntries(1000)
        , Policy(ECachePolicy::LRU)
        , TTL(3600.0f) // 1 hour default
        , bEnableCompression(true)
        , bEnablePersistence(true)
        , bEnablePreloading(true)
    {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCacheHit, const FString&, Key);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCacheMiss, const FString&, Key);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCacheEvicted, const FString&, Key);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCacheStatisticsUpdated, const FCacheStatistics&, Statistics);

UCLASS(Blueprintable)
class IGED_UE5_API UIGEDAdvancedCaching : public UObject
{
    GENERATED_BODY()

public:
    UIGEDAdvancedCaching();

    // Initialization
    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    void Initialize(const FCacheConfiguration& Config);

    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    void Shutdown();

    // Cache operations
    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    bool Put(const FString& Key, const FString& Data, float Priority = 1.0f, bool bIsPersistent = false);

    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    FString Get(const FString& Key);

    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    bool Contains(const FString& Key);

    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    bool Remove(const FString& Key);

    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    void Clear();

    // Advanced operations
    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    void PreloadData(const TArray<FString>& Keys);

    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    void WarmupCache();

    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    void OptimizeCache();

    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    void CompressCache();

    // Statistics and monitoring
    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    FCacheStatistics GetStatistics();

    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    TArray<FCacheEntry> GetCacheEntries();

    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    void UpdateStatistics();

    // Configuration
    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    void SetConfiguration(const FCacheConfiguration& Config);

    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    FCacheConfiguration GetConfiguration() const;

    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    void SetCachePolicy(ECachePolicy Policy);

    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    void SetMaxSize(int32 MaxSize);

    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    void SetTTL(float TTL);

    // Persistence
    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    void SaveCacheToDisk(const FString& FilePath);

    UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
    void LoadCacheFromDisk(const FString& FilePath);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "IGED|Caching|Events")
    FOnCacheHit OnCacheHit;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Caching|Events")
    FOnCacheMiss OnCacheMiss;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Caching|Events")
    FOnCacheEvicted OnCacheEvicted;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Caching|Events")
    FOnCacheStatisticsUpdated OnCacheStatisticsUpdated;

protected:
    UPROPERTY()
    bool bIsInitialized;

    UPROPERTY()
    FCacheConfiguration Configuration;

    UPROPERTY()
    TMap<FString, FCacheEntry> CacheEntries;

    UPROPERTY()
    FCacheStatistics Statistics;

    UPROPERTY()
    int32 TotalHits;

    UPROPERTY()
    int32 TotalMisses;

private:
    void EvictEntry();
    void UpdateAccessTime(const FString& Key);
    void UpdateAccessCount(const FString& Key);
    bool IsExpired(const FCacheEntry& Entry) const;
    void CleanupExpiredEntries();
    void UpdateStatistics();
    
    // Eviction strategies
    FString FindLRUEntry();
    FString FindLFUEntry();
    FString FindFIFOEntry();
    FString FindLIFOEntry();
    FString FindPriorityEntry();
    FString FindAdaptiveEntry();
    
    // Utility functions
    int32 CalculateEntrySize(const FCacheEntry& Entry) const;
    void CompressEntry(FCacheEntry& Entry);
    void DecompressEntry(FCacheEntry& Entry);
    
    // Timer handles
    FTimerHandle CleanupTimer;
    FTimerHandle StatisticsTimer;
};

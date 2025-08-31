// IGED Resource Manager - Intelligent Resource Management System
// Copyright (c) 2024 Project Human Bot

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IGEDResourceManager.generated.h"

// Resource structure
USTRUCT(BlueprintType)
struct FResource
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    FString ResourceID;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    FString ResourceName;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    FString ResourceType;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    float CurrentUsage;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    float MaxCapacity;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    float Allocation;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    bool bIsAvailable;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    int32 Priority;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    FDateTime LastAccessed;

    FResource()
        : CurrentUsage(0.0f)
        , MaxCapacity(0.0f)
        , Allocation(0.0f)
        , bIsAvailable(true)
        , Priority(1)
    {}
};

// Resource allocation structure
USTRUCT(BlueprintType)
struct FResourceAllocation
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    FString AllocationID;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    FString ResourceID;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    FString ProcessID;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    float AllocatedAmount;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    FDateTime AllocationTime;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    FDateTime ExpirationTime;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    bool bIsActive;

    FResourceAllocation()
        : AllocatedAmount(0.0f)
        , bIsActive(false)
    {}
};

// Resource statistics structure
USTRUCT(BlueprintType)
struct FResourceStatistics
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    int32 TotalResources;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    int32 AvailableResources;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    int32 AllocatedResources;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    float TotalCapacity;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    float UsedCapacity;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    float AvailableCapacity;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    float UtilizationRate;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    int32 ActiveAllocations;

    FResourceStatistics()
        : TotalResources(0)
        , AvailableResources(0)
        , AllocatedResources(0)
        , TotalCapacity(0.0f)
        , UsedCapacity(0.0f)
        , AvailableCapacity(0.0f)
        , UtilizationRate(0.0f)
        , ActiveAllocations(0)
    {}
};

// Resource type enum
UENUM(BlueprintType)
enum class EResourceType : uint8
{
    CPU UMETA(DisplayName = "CPU"),
    Memory UMETA(DisplayName = "Memory"),
    GPU UMETA(DisplayName = "GPU"),
    Network UMETA(DisplayName = "Network"),
    Storage UMETA(DisplayName = "Storage"),
    Thread UMETA(DisplayName = "Thread"),
    Custom UMETA(DisplayName = "Custom")
};

// Resource priority enum
UENUM(BlueprintType)
enum class EResourcePriority : uint8
{
    Low UMETA(DisplayName = "Low"),
    Normal UMETA(DisplayName = "Normal"),
    High UMETA(DisplayName = "High"),
    Critical UMETA(DisplayName = "Critical")
};

// Resource manager configuration
USTRUCT(BlueprintType)
struct FResourceManagerConfiguration
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    bool bEnableAutoAllocation;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    bool bEnableLoadBalancing;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    bool bEnableResourcePooling;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    bool bEnableDynamicScaling;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    float MaxUtilizationThreshold;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    float MinUtilizationThreshold;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    int32 MaxConcurrentAllocations;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Resources")
    float AllocationTimeout;

    FResourceManagerConfiguration()
        : bEnableAutoAllocation(true)
        , bEnableLoadBalancing(true)
        , bEnableResourcePooling(true)
        , bEnableDynamicScaling(true)
        , MaxUtilizationThreshold(0.8f)
        , MinUtilizationThreshold(0.2f)
        , MaxConcurrentAllocations(100)
        , AllocationTimeout(30.0f)
    {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceAllocated, const FResourceAllocation&, Allocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceDeallocated, const FResourceAllocation&, Allocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceExhausted, const FResource&, Resource);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceStatisticsUpdated, const FResourceStatistics&, Statistics);

UCLASS(Blueprintable)
class IGED_UE5_API UIGEDResourceManager : public UObject
{
    GENERATED_BODY()

public:
    UIGEDResourceManager();

    // Initialization
    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    void Initialize(const FResourceManagerConfiguration& Config);

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    void Shutdown();

    // Resource management
    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    FString RegisterResource(const FString& ResourceName, EResourceType Type, float MaxCapacity, EResourcePriority Priority = EResourcePriority::Normal);

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    bool UnregisterResource(const FString& ResourceID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    FResource GetResource(const FString& ResourceID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    TArray<FResource> GetAllResources();

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    TArray<FResource> GetResourcesByType(EResourceType Type);

    // Resource allocation
    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    FString AllocateResource(const FString& ResourceID, const FString& ProcessID, float Amount, float Duration = 0.0f);

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    bool DeallocateResource(const FString& AllocationID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    bool DeallocateResourcesByProcess(const FString& ProcessID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    FResourceAllocation GetAllocation(const FString& AllocationID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    TArray<FResourceAllocation> GetAllocationsByResource(const FString& ResourceID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    TArray<FResourceAllocation> GetAllocationsByProcess(const FString& ProcessID);

    // Resource optimization
    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    void OptimizeResourceAllocation();

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    void BalanceResourceLoad();

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    void ScaleResources();

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    void PoolResources();

    // Resource monitoring
    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    void StartResourceMonitoring();

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    void StopResourceMonitoring();

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    void UpdateResourceUsage();

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    FResourceStatistics GetStatistics();

    // Advanced features
    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    FString FindBestResource(EResourceType Type, float RequiredAmount, EResourcePriority Priority = EResourcePriority::Normal);

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    bool CanAllocateResource(const FString& ResourceID, float Amount);

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    float GetAvailableCapacity(const FString& ResourceID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    float GetUtilizationRate(const FString& ResourceID);

    // Configuration
    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    void SetConfiguration(const FResourceManagerConfiguration& Config);

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    FResourceManagerConfiguration GetConfiguration() const;

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    void SetMaxUtilizationThreshold(float Threshold);

    UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
    void SetMinUtilizationThreshold(float Threshold);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "IGED|Resources|Events")
    FOnResourceAllocated OnResourceAllocated;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Resources|Events")
    FOnResourceDeallocated OnResourceDeallocated;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Resources|Events")
    FOnResourceExhausted OnResourceExhausted;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Resources|Events")
    FOnResourceStatisticsUpdated OnResourceStatisticsUpdated;

protected:
    UPROPERTY()
    bool bIsInitialized;

    UPROPERTY()
    bool bIsMonitoring;

    UPROPERTY()
    FResourceManagerConfiguration Configuration;

    UPROPERTY()
    TMap<FString, FResource> Resources;

    UPROPERTY()
    TMap<FString, FResourceAllocation> Allocations;

    UPROPERTY()
    FResourceStatistics Statistics;

    UPROPERTY()
    TMap<FString, TArray<FString>> ProcessAllocations;

private:
    void UpdateStatistics();
    void CleanupExpiredAllocations();
    void OptimizeResourceDistribution();
    void CheckResourceThresholds();
    
    // Resource allocation algorithms
    FString FindBestResourceForAllocation(EResourceType Type, float Amount, EResourcePriority Priority);
    bool AllocateResourceInternal(const FString& ResourceID, const FString& ProcessID, float Amount, float Duration);
    void DeallocateResourceInternal(const FString& AllocationID);
    
    // Resource optimization
    void OptimizeResourcePool();
    void BalanceResourceUtilization();
    void ScaleResourceCapacity();
    void PoolResourceAllocations();
    
    // Utility functions
    float CalculateResourceUtilization(const FResource& Resource);
    bool IsResourceAvailable(const FResource& Resource, float Amount);
    void UpdateResourceUsage(const FString& ResourceID);
    void NotifyResourceExhausted(const FResource& Resource);
    
    // Timer handles
    FTimerHandle ResourceMonitoringTimer;
    FTimerHandle StatisticsTimer;
    FTimerHandle CleanupTimer;
    FTimerHandle OptimizationTimer;
};

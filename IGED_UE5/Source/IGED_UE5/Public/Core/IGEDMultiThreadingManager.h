// IGED Multi-Threading Manager - Advanced Threading and Async Processing
// Copyright (c) 2024 Project Human Bot

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/ThreadSafeBool.h"
#include "IGEDMultiThreadingManager.generated.h"

// Task structure
USTRUCT(BlueprintType)
struct FThreadTask
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    FString TaskID;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    FString TaskName;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    int32 Priority;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    float EstimatedDuration;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    bool bIsCompleted;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    bool bIsRunning;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    FDateTime CreatedTime;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    FDateTime StartTime;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    FDateTime EndTime;

    FThreadTask()
        : Priority(1)
        , EstimatedDuration(0.0f)
        , bIsCompleted(false)
        , bIsRunning(false)
    {}
};

// Thread pool statistics
USTRUCT(BlueprintType)
struct FThreadPoolStatistics
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    int32 TotalThreads;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    int32 ActiveThreads;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    int32 IdleThreads;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    int32 QueuedTasks;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    int32 CompletedTasks;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    float AverageTaskDuration;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    float CPUUtilization;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    float MemoryUsage;

    FThreadPoolStatistics()
        : TotalThreads(0)
        , ActiveThreads(0)
        , IdleThreads(0)
        , QueuedTasks(0)
        , CompletedTasks(0)
        , AverageTaskDuration(0.0f)
        , CPUUtilization(0.0f)
        , MemoryUsage(0.0f)
    {}
};

// Task priority enum
UENUM(BlueprintType)
enum class ETaskPriority : uint8
{
    Low UMETA(DisplayName = "Low"),
    Normal UMETA(DisplayName = "Normal"),
    High UMETA(DisplayName = "High"),
    Critical UMETA(DisplayName = "Critical")
};

// Thread pool configuration
USTRUCT(BlueprintType)
struct FThreadPoolConfiguration
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    int32 MinThreads;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    int32 MaxThreads;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    int32 MaxQueuedTasks;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    float ThreadIdleTimeout;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    bool bEnableTaskStealing;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    bool bEnableLoadBalancing;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Threading")
    bool bEnablePriorityQueuing;

    FThreadPoolConfiguration()
        : MinThreads(2)
        , MaxThreads(FPlatformMisc::NumberOfCores())
        , MaxQueuedTasks(1000)
        , ThreadIdleTimeout(30.0f)
        , bEnableTaskStealing(true)
        , bEnableLoadBalancing(true)
        , bEnablePriorityQueuing(true)
    {}
};

// Custom runnable class for IGED tasks
class IGED_UE5_API FIGEDTaskRunnable : public FRunnable
{
public:
    FIGEDTaskRunnable(const FThreadTask& InTask, UIGEDMultiThreadingManager* InManager);
    virtual ~FIGEDTaskRunnable();

    // FRunnable interface
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;
    virtual void Exit() override;

    FThreadTask GetTask() const { return Task; }
    bool IsRunning() const { return bIsRunning; }

private:
    FThreadTask Task;
    UIGEDMultiThreadingManager* Manager;
    FThreadSafeBool bIsRunning;
    FThreadSafeBool bShouldStop;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTaskStarted, const FThreadTask&, Task);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTaskCompleted, const FThreadTask&, Task);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTaskFailed, const FThreadTask&, Task);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnThreadPoolStatisticsUpdated, const FThreadPoolStatistics&, Statistics);

UCLASS(Blueprintable)
class IGED_UE5_API UIGEDMultiThreadingManager : public UObject
{
    GENERATED_BODY()

public:
    UIGEDMultiThreadingManager();

    // Initialization
    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    void Initialize(const FThreadPoolConfiguration& Config);

    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    void Shutdown();

    // Task management
    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    FString SubmitTask(const FString& TaskName, ETaskPriority Priority = ETaskPriority::Normal);

    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    bool CancelTask(const FString& TaskID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    bool IsTaskCompleted(const FString& TaskID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    FThreadTask GetTask(const FString& TaskID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    TArray<FThreadTask> GetActiveTasks();

    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    TArray<FThreadTask> GetQueuedTasks();

    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    TArray<FThreadTask> GetCompletedTasks();

    // Thread pool management
    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    void SetThreadPoolSize(int32 MinThreads, int32 MaxThreads);

    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    void EnableTaskStealing(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    void EnableLoadBalancing(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    void EnablePriorityQueuing(bool bEnable);

    // Statistics and monitoring
    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    FThreadPoolStatistics GetStatistics();

    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    void UpdateStatistics();

    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    void StartMonitoring();

    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    void StopMonitoring();

    // Advanced features
    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    void OptimizeThreadPool();

    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    void BalanceLoad();

    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    void StealTasks();

    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    void CleanupCompletedTasks();

    // Configuration
    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    void SetConfiguration(const FThreadPoolConfiguration& Config);

    UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
    FThreadPoolConfiguration GetConfiguration() const;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "IGED|Threading|Events")
    FOnTaskStarted OnTaskStarted;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Threading|Events")
    FOnTaskCompleted OnTaskCompleted;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Threading|Events")
    FOnTaskFailed OnTaskFailed;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Threading|Events")
    FOnThreadPoolStatisticsUpdated OnThreadPoolStatisticsUpdated;

    // Internal methods for runnable classes
    void OnTaskStartedInternal(const FThreadTask& Task);
    void OnTaskCompletedInternal(const FThreadTask& Task);
    void OnTaskFailedInternal(const FThreadTask& Task);

protected:
    UPROPERTY()
    bool bIsInitialized;

    UPROPERTY()
    bool bIsMonitoring;

    UPROPERTY()
    FThreadPoolConfiguration Configuration;

    UPROPERTY()
    TMap<FString, FThreadTask> Tasks;

    UPROPERTY()
    TArray<FString> TaskQueue;

    UPROPERTY()
    TArray<FString> ActiveTasks;

    UPROPERTY()
    TArray<FString> CompletedTasks;

    UPROPERTY()
    FThreadPoolStatistics Statistics;

    UPROPERTY()
    TArray<FRunnableThread*> ThreadPool;

    UPROPERTY()
    TArray<FIGEDTaskRunnable*> RunnableTasks;

private:
    void ProcessTaskQueue();
    void CreateThread();
    void DestroyThread();
    void UpdateStatistics();
    void CleanupThreads();
    
    // Task scheduling
    FString GetNextTask();
    void ScheduleTask(const FString& TaskID);
    void CompleteTask(const FString& TaskID);
    void FailTask(const FString& TaskID);
    
    // Thread management
    void ManageThreadPool();
    void BalanceThreads();
    void OptimizeThreads();
    
    // Utility functions
    int32 GetTaskPriority(const FString& TaskID) const;
    float GetTaskDuration(const FString& TaskID) const;
    bool IsTaskExpired(const FString& TaskID) const;
    
    // Thread synchronization
    FCriticalSection TaskQueueMutex;
    FCriticalSection StatisticsMutex;
    FCriticalSection ThreadPoolMutex;
    
    // Timer handles
    FTimerHandle TaskProcessingTimer;
    FTimerHandle StatisticsTimer;
    FTimerHandle ThreadManagementTimer;
};

// IGED Base Agent - Abstract Agent Interface
// Copyright (c) 2024 Project Human Bot

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "IGEDBaseAgent.generated.h"

// Forward declarations
class UIGEDMemoryEngine;

UENUM(BlueprintType)
enum class EAgentType : uint8
{
	SecOps			UMETA(DisplayName = "Security Operations"),
	NetworkIntel	UMETA(DisplayName = "Network Intelligence"),
	DataMiner		UMETA(DisplayName = "Data Mining"),
	CodeGen			UMETA(DisplayName = "Code Generation"),
	RemoteControl	UMETA(DisplayName = "Remote Control"),
	Custom			UMETA(DisplayName = "Custom Plugin")
};

UENUM(BlueprintType)
enum class EAgentPriority : uint8
{
	Low,
	Normal,
	High,
	Critical
};

USTRUCT(BlueprintType)
struct FAgentCapability
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CapabilityName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> RequiredPermissions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ConfidenceScore;

	FAgentCapability()
	{
		ConfidenceScore = 1.0f;
	}
};

USTRUCT(BlueprintType)
struct FAgentTask
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString TaskID;

	UPROPERTY(BlueprintReadOnly)
	FString Command;

	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FString> Parameters;

	UPROPERTY(BlueprintReadOnly)
	EAgentPriority Priority;

	UPROPERTY(BlueprintReadOnly)
	float TimeoutSeconds;

	UPROPERTY(BlueprintReadOnly)
	FDateTime StartTime;

	FAgentTask()
	{
		Priority = EAgentPriority::Normal;
		TimeoutSeconds = 30.0f;
		TaskID = FGuid::NewGuid().ToString();
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAgentTaskCompleted, const FString&, TaskID, bool, bSuccess, const FString&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAgentStateChanged, EAgentType, AgentType, bool, bIsActive);

UCLASS(Abstract, Blueprintable)
class IGED_UE5_API UIGEDBaseAgent : public UObject
{
	GENERATED_BODY()

public:
	UIGEDBaseAgent();

	// Core interface
	UFUNCTION(BlueprintCallable, Category = "IGED|Agent")
	virtual void Initialize(UIGEDMemoryEngine* InMemoryEngine);

	UFUNCTION(BlueprintCallable, Category = "IGED|Agent")
	virtual void Shutdown();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IGED|Agent")
	FString ExecuteTask(const FAgentTask& Task);
	virtual FString ExecuteTask_Implementation(const FAgentTask& Task);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IGED|Agent")
	bool CanHandleCommand(const FString& Command) const;
	virtual bool CanHandleCommand_Implementation(const FString& Command) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "IGED|Agent")
	float GetConfidenceScore(const FString& Command) const;
	virtual float GetConfidenceScore_Implementation(const FString& Command) const;

	// Agent state management
	UFUNCTION(BlueprintCallable, Category = "IGED|Agent")
	void SetActive(bool bNewActive);

	UFUNCTION(BlueprintCallable, Category = "IGED|Agent")
	bool IsActive() const { return bIsActive; }

	UFUNCTION(BlueprintCallable, Category = "IGED|Agent")
	void SetPriority(EAgentPriority NewPriority);

	// Capabilities
	UFUNCTION(BlueprintCallable, Category = "IGED|Agent")
	TArray<FAgentCapability> GetCapabilities() const { return Capabilities; }

	UFUNCTION(BlueprintCallable, Category = "IGED|Agent")
	void AddCapability(const FAgentCapability& NewCapability);

	// Statistics
	UFUNCTION(BlueprintCallable, Category = "IGED|Agent")
	int32 GetTasksCompleted() const { return TasksCompleted; }

	UFUNCTION(BlueprintCallable, Category = "IGED|Agent")
	float GetSuccessRate() const;

	UFUNCTION(BlueprintCallable, Category = "IGED|Agent")
	float GetAverageExecutionTime() const { return AverageExecutionTime; }

	// Configuration
	UFUNCTION(BlueprintCallable, Category = "IGED|Agent")
	void LoadConfiguration(const FString& ConfigPath);

	UFUNCTION(BlueprintCallable, Category = "IGED|Agent")
	void SaveConfiguration(const FString& ConfigPath);

	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Agent")
	EAgentType AgentType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Agent")
	FString AgentName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Agent")
	FString AgentVersion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Agent")
	EAgentPriority Priority;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Agent")
	float DefaultTimeout;

	// Events
	UPROPERTY(BlueprintAssignable, Category = "IGED|Events")
	FOnAgentTaskCompleted OnTaskCompleted;

	UPROPERTY(BlueprintAssignable, Category = "IGED|Events")
	FOnAgentStateChanged OnStateChanged;

protected:
	// Memory engine reference
	UPROPERTY()
	UIGEDMemoryEngine* MemoryEngine;

	// Agent capabilities
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Agent")
	TArray<FAgentCapability> Capabilities;

	// State
	UPROPERTY(BlueprintReadOnly, Category = "Agent")
	bool bIsActive;

	// Active tasks
	TMap<FString, FAgentTask> ActiveTasks;
	FCriticalSection TaskMutex;

	// Statistics
	int32 TasksCompleted;
	int32 TasksFailed;
	float TotalExecutionTime;
	float AverageExecutionTime;

	// Helper methods
	virtual void LogToMemory(const FString& Action, const FString& Details, bool bSuccess);
	virtual FString ProcessTaskInternal(const FAgentTask& Task);
	void UpdateStatistics(bool bSuccess, float ExecutionTime);

private:
	FDateTime LastActivityTime;
};

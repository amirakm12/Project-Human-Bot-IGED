// IGED Orchestrator - Agent Management System
// Copyright (c) 2024 Project Human Bot

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Dom/JsonObject.h"
#include "IGEDOrchestrator.generated.h"

// Forward declarations
class UIGEDBaseAgent;
class UIGEDSecOpsAgent;
class UIGEDNetworkIntelAgent;
class UIGEDDataMinerAgent;
class UIGEDCodeGenAgent;
class UIGEDRemoteControlAgent;
class UIGEDRAGOrchestratorAgent;
class UIGEDModelManager;
class UIGEDMemoryEngine;

USTRUCT(BlueprintType)
struct FAgentStatus
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString AgentName;

	UPROPERTY(BlueprintReadOnly)
	bool bIsActive;

	UPROPERTY(BlueprintReadOnly)
	int32 TasksCompleted;

	UPROPERTY(BlueprintReadOnly)
	float SuccessRate;

	UPROPERTY(BlueprintReadOnly)
	FString LastTaskTime;

	FAgentStatus()
	{
		bIsActive = false;
		TasksCompleted = 0;
		SuccessRate = 0.0f;
	}
};

USTRUCT(BlueprintType)
struct FCommandResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bSuccess;

	UPROPERTY(BlueprintReadOnly)
	FString Result;

	UPROPERTY(BlueprintReadOnly)
	FString AgentUsed;

	UPROPERTY(BlueprintReadOnly)
	float ExecutionTime;

	FCommandResult()
	{
		bSuccess = false;
		ExecutionTime = 0.0f;
	}
};

UCLASS(Blueprintable)
class IGED_UE5_API UIGEDOrchestrator : public UObject
{
	GENERATED_BODY()

public:
	UIGEDOrchestrator();

	// Initialization
	UFUNCTION(BlueprintCallable, Category = "IGED|Orchestrator")
	void Initialize(UIGEDMemoryEngine* InMemoryEngine, UIGEDModelManager* InModelManager);

	// Command execution
	UFUNCTION(BlueprintCallable, Category = "IGED|Orchestrator")
	FString ExecuteCommand(const FString& Command);

	UFUNCTION(BlueprintCallable, Category = "IGED|Orchestrator")
	FCommandResult ExecuteCommandWithDetails(const FString& Command);

	// Agent management
	UFUNCTION(BlueprintCallable, Category = "IGED|Orchestrator")
	void EnableAgent(const FString& AgentName);

	UFUNCTION(BlueprintCallable, Category = "IGED|Orchestrator")
	void DisableAgent(const FString& AgentName);

	UFUNCTION(BlueprintCallable, Category = "IGED|Orchestrator")
	void ReloadAgent(const FString& AgentName);

	UFUNCTION(BlueprintCallable, Category = "IGED|Orchestrator")
	TArray<FString> GetAvailableAgents() const;

	UFUNCTION(BlueprintCallable, Category = "IGED|Orchestrator")
	FAgentStatus GetAgentStatus(const FString& AgentName) const;

	// Plugin management
	UFUNCTION(BlueprintCallable, Category = "IGED|Orchestrator")
	void LoadPlugin(const FString& PluginPath);

	UFUNCTION(BlueprintCallable, Category = "IGED|Orchestrator")
	void UnloadPlugin(const FString& PluginName);

	UFUNCTION(BlueprintCallable, Category = "IGED|Orchestrator")
	TArray<FString> GetLoadedPlugins() const;

	// Task routing
	UFUNCTION(BlueprintCallable, Category = "IGED|Orchestrator")
	FString RouteTaskToAgent(const FString& Task, const FString& PreferredAgent);

	// Batch operations
	UFUNCTION(BlueprintCallable, Category = "IGED|Orchestrator")
	TArray<FCommandResult> ExecuteBatch(const TArray<FString>& Commands);

	// HTTP response processing
	void ProcessHTTPResponse(const FString& ResponseContent);

	// Status and monitoring
	TSharedPtr<FJsonObject> GetStatus() const;

	UFUNCTION(BlueprintCallable, Category = "IGED|Orchestrator")
	int32 GetActiveTaskCount() const;

	UFUNCTION(BlueprintCallable, Category = "IGED|Orchestrator")
	float GetSystemLoad() const;

protected:
	// Agent instances
	UPROPERTY()
	TMap<FString, UIGEDBaseAgent*> Agents;

	UPROPERTY()
	UIGEDSecOpsAgent* SecOpsAgent;

	UPROPERTY()
	UIGEDNetworkIntelAgent* NetworkIntelAgent;

	UPROPERTY()
	UIGEDDataMinerAgent* DataMinerAgent;

	UPROPERTY()
	UIGEDCodeGenAgent* CodeGenAgent;

	UPROPERTY()
	UIGEDRemoteControlAgent* RemoteControlAgent;

	UPROPERTY()
	UIGEDRAGOrchestratorAgent* RAGOrchestratorAgent;

	// Core system references
	UPROPERTY()
	UIGEDMemoryEngine* MemoryEngine;

	UPROPERTY()
	UIGEDModelManager* ModelManager;

	// Plugin system
	TMap<FString, TSharedPtr<class IIGEDPlugin>> LoadedPlugins;

	// Task management
	TQueue<FString> TaskQueue;
	TArray<FString> ActiveTasks;
	FCriticalSection TaskQueueMutex;

	// Statistics
	int32 TotalTasksProcessed;
	int32 SuccessfulTasks;
	float AverageExecutionTime;

private:
	void LoadBuiltInAgents();
	void LoadAgentFromClass(TSubclassOf<UIGEDBaseAgent> AgentClass, const FString& AgentName);
	UIGEDBaseAgent* SelectBestAgent(const FString& Command);
	FString ParseCommandType(const FString& Command) const;
	void UpdateStatistics(bool bSuccess, float ExecutionTime);

	// Agent initialization helpers
	void InitializeSecOpsAgent();
	void InitializeNetworkIntelAgent();
	void InitializeDataMinerAgent();
	void InitializeCodeGenAgent();
	void InitializeRemoteControlAgent();
	void InitializeRAGOrchestratorAgent();
};

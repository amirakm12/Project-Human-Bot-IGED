// IGED Game Instance - Core Application Manager
// Copyright (c) 2024 Project Human Bot

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Http.h"
#include "Dom/JsonObject.h"
#include "IGEDGameInstance.generated.h"

// Forward declarations
class UIGEDOrchestrator;
class UIGEDVoicePipeline;
class UIGEDMemoryEngine;
class UIGEDEncryptionManager;
class UIGEDBiometricAuth;
class UIGEDModelManager;
class UIGEDExplainableAI;
class UIGEDVectorDatabase;
class UIGEDLogger;
class UIGEDPressureTester;
class UIGEDSystemValidator;
class UIGEDPerformanceOptimizer;
class UIGEDAnalyticsDashboard;
class UIGEDAdvancedCaching;
class UIGEDMultiThreadingManager;
class UIGEDAdvancedSecurity;
class UIGEDResourceManager;
class UIGEDAdvancedUI;
class UIGEDMLOptimizer;
class UIGEDAdvancedNetworking;
class UIGEDCollaborationManager;
class UIGEDMainHUD;
class UNiagaraSystem;
class UNiagaraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCommandExecuted, const FString&, Command, const FString&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSystemStatusChanged, const FString&, Status);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAgentStateChanged, const FString&, AgentName, bool, bIsActive);

UENUM(BlueprintType)
enum class EIGEDSystemState : uint8
{
	Uninitialized,
	Initializing,
	Ready,
	Processing,
	Error,
	ShuttingDown
};

UCLASS()
class IGED_UE5_API UIGEDGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UIGEDGameInstance();

	// UGameInstance interface
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void StartGameInstance() override;

	// Core system functions
	UFUNCTION(BlueprintCallable, Category = "IGED|Core")
	void InitializeSystem();

	UFUNCTION(BlueprintCallable, Category = "IGED|Core")
	void ShutdownSystem();

	UFUNCTION(BlueprintCallable, Category = "IGED|Core")
	bool ExecuteCommand(const FString& Command);

	UFUNCTION(BlueprintCallable, Category = "IGED|Core")
	FString GetSystemStatus() const;

	UFUNCTION(BlueprintCallable, Category = "IGED|Core")
	void SetSystemState(EIGEDSystemState NewState);

	// Agent management
	UFUNCTION(BlueprintCallable, Category = "IGED|Agents")
	void EnableAgent(const FString& AgentName);

	UFUNCTION(BlueprintCallable, Category = "IGED|Agents")
	void DisableAgent(const FString& AgentName);

	UFUNCTION(BlueprintCallable, Category = "IGED|Agents")
	TArray<FString> GetAvailableAgents() const;

	// Voice control
	UFUNCTION(BlueprintCallable, Category = "IGED|Voice")
	void StartVoiceListening();

	UFUNCTION(BlueprintCallable, Category = "IGED|Voice")
	void StopVoiceListening();

	UFUNCTION(BlueprintCallable, Category = "IGED|Voice")
	bool IsVoiceListening() const;

	// Networking
	UFUNCTION(BlueprintCallable, Category = "IGED|Network")
	void SendHTTPRequest(const FString& URL, const FString& Verb, const FString& Content);

	// Biometric authentication
	UFUNCTION(BlueprintCallable, Category = "IGED|Security")
	void AuthenticateWithBiometrics();

	UFUNCTION(BlueprintCallable, Category = "IGED|Security")
	bool IsBiometricAuthenticationAvailable() const;

	// Explainable AI
	UFUNCTION(BlueprintCallable, Category = "IGED|XAI")
	void InitializeExplainableAI();

	UFUNCTION(BlueprintCallable, Category = "IGED|XAI")
	void GenerateModelExplanation(const FString& ModelOutput, const FString& InputPrompt);

	UFUNCTION(BlueprintCallable, Category = "IGED|XAI")
	void StartRetrievalAugmentedFineTuning(const FString& BaseModel, const FString& TrainingData);

	// Vector Database
	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void InitializeVectorDatabase();

	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void AddDocumentToVectorDB(const FString& DocumentContent, const FString& DocumentID);

	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void SearchVectorDatabase(const FString& Query, int32 MaxResults);

	// Logging and Testing
	UFUNCTION(BlueprintCallable, Category = "IGED|Logging")
	void InitializeLoggingSystem();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing")
	void RunComprehensiveTests();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing")
	void RunSystemHealthCheck();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing")
	FString GetSystemHealthReport();

	// Advanced Performance and Analytics
	UFUNCTION(BlueprintCallable, Category = "IGED|Performance")
	void InitializePerformanceOptimizer();

	UFUNCTION(BlueprintCallable, Category = "IGED|Analytics")
	void InitializeAnalyticsDashboard();

	UFUNCTION(BlueprintCallable, Category = "IGED|Caching")
	void InitializeAdvancedCaching();

	UFUNCTION(BlueprintCallable, Category = "IGED|Threading")
	void InitializeMultiThreadingManager();

	UFUNCTION(BlueprintCallable, Category = "IGED|Security")
	void InitializeAdvancedSecurity();

	UFUNCTION(BlueprintCallable, Category = "IGED|Resources")
	void InitializeResourceManager();

	UFUNCTION(BlueprintCallable, Category = "IGED|UI")
	void InitializeAdvancedUI();

	UFUNCTION(BlueprintCallable, Category = "IGED|ML")
	void InitializeMLOptimizer();

	UFUNCTION(BlueprintCallable, Category = "IGED|Networking")
	void InitializeAdvancedNetworking();

	UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
	void InitializeCollaborationManager();

	// Visualization control
	UFUNCTION(BlueprintCallable, Category = "IGED|Visualization")
	void EnableDataVisualization(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "IGED|Visualization")
	void SetVisualizationIntensity(float Intensity);

	// Configuration
	UFUNCTION(BlueprintCallable, Category = "IGED|Config")
	void LoadConfiguration(const FString& ConfigPath);

	UFUNCTION(BlueprintCallable, Category = "IGED|Config")
	void SaveConfiguration(const FString& ConfigPath);

	// Properties
	UPROPERTY(BlueprintReadOnly, Category = "IGED|Core")
	EIGEDSystemState CurrentSystemState;

	UPROPERTY(BlueprintReadOnly, Category = "IGED|Core")
	FString SystemVersion;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Config")
	bool bOfflineMode;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Config")
	bool bDebugMode;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Config")
	float VoiceSensitivity;

	// Events
	UPROPERTY(BlueprintAssignable, Category = "IGED|Events")
	FOnCommandExecuted OnCommandExecuted;

	UPROPERTY(BlueprintAssignable, Category = "IGED|Events")
	FOnSystemStatusChanged OnSystemStatusChanged;

	UPROPERTY(BlueprintAssignable, Category = "IGED|Events")
	FOnAgentStateChanged OnAgentStateChanged;

protected:
	// Core components
	UPROPERTY()
	UIGEDOrchestrator* Orchestrator;

	UPROPERTY()
	UIGEDVoicePipeline* VoicePipeline;

	UPROPERTY()
	UIGEDMemoryEngine* MemoryEngine;

	UPROPERTY()
	UIGEDEncryptionManager* EncryptionManager;

	UPROPERTY()
	UIGEDBiometricAuth* BiometricAuth;

	UPROPERTY()
	UIGEDModelManager* ModelManager;

	UPROPERTY()
	UIGEDExplainableAI* ExplainableAI;

	UPROPERTY()
	UIGEDVectorDatabase* VectorDatabase;

	UPROPERTY()
	UIGEDLogger* Logger;

	UPROPERTY()
	UIGEDPressureTester* PressureTester;

	UPROPERTY()
	UIGEDSystemValidator* SystemValidator;

	UPROPERTY()
	UIGEDPerformanceOptimizer* PerformanceOptimizer;

	UPROPERTY()
	UIGEDAnalyticsDashboard* AnalyticsDashboard;

	UPROPERTY()
	UIGEDAdvancedCaching* AdvancedCaching;

	UPROPERTY()
	UIGEDMultiThreadingManager* MultiThreadingManager;

	UPROPERTY()
	UIGEDAdvancedSecurity* AdvancedSecurity;

	UPROPERTY()
	UIGEDResourceManager* ResourceManager;

	UPROPERTY()
	UIGEDAdvancedUI* AdvancedUI;

	UPROPERTY()
	UIGEDMLOptimizer* MLOptimizer;

	UPROPERTY()
	UIGEDAdvancedNetworking* AdvancedNetworking;

	UPROPERTY()
	UIGEDCollaborationManager* CollaborationManager;

	// UI Components
	UPROPERTY()
	UIGEDMainHUD* MainHUD;

	// Visualization components
	UPROPERTY()
	UNiagaraSystem* DataFlowEffect;

	UPROPERTY()
	UNiagaraComponent* ActiveNiagaraComponent;

	// HTTP handling
	FHttpModule* HttpModule;
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> ActiveRequest;

	// Configuration
	TSharedPtr<FJsonObject> ConfigurationData;

private:
	void InitializeComponents();
	void InitializeVisualization();
	void InitializeNetworking();
	void OnHttpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void ProcessVoiceCommand(const FString& Command);
	void UpdateSystemMetrics();

	// Thread safety
	FCriticalSection CommandQueueMutex;
	TQueue<FString> CommandQueue;

	// Timers
	FTimerHandle MetricsUpdateTimer;
	FTimerHandle AutoSaveTimer;

	// Performance metrics
	float CurrentFPS;
	float MemoryUsageMB;
	int32 ActiveThreadCount;
};

// IGED Pressure Testing Framework - Comprehensive System Validation
// Copyright (c) 2024 Project Human Bot

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Dom/JsonObject.h"
#include "IGEDPressureTester.generated.h"

// Test result structure
USTRUCT(BlueprintType)
struct FIGEDTestResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Testing")
	FString TestName;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Testing")
	bool bPassed;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Testing")
	FString ErrorMessage;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Testing")
	float ExecutionTime;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Testing")
	int32 Iterations;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Testing")
	TMap<FString, FString> Metrics;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Testing")
	FDateTime TestTime;

	FIGEDTestResult()
		: bPassed(false)
		, ExecutionTime(0.0f)
		, Iterations(0)
		, TestTime(FDateTime::Now())
	{}
};

// Test configuration
USTRUCT(BlueprintType)
struct FIGEDTestConfig
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Testing")
	int32 MaxIterations;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Testing")
	float TimeoutSeconds;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Testing")
	bool bRunConcurrentTests;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Testing")
	int32 ConcurrentThreads;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Testing")
	bool bStopOnFirstFailure;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Testing")
	bool bGenerateDetailedReport;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Testing")
	FString ReportOutputPath;

	FIGEDTestConfig()
		: MaxIterations(1000)
		, TimeoutSeconds(30.0f)
		, bRunConcurrentTests(false)
		, ConcurrentThreads(4)
		, bStopOnFirstFailure(false)
		, bGenerateDetailedReport(true)
		, ReportOutputPath(TEXT("TestReports"))
	{}
};

// Test categories
UENUM(BlueprintType)
enum class EIGEDTestCategory : uint8
{
	Core UMETA(DisplayName = "Core System"),
	AI UMETA(DisplayName = "AI Models"),
	Agents UMETA(DisplayName = "Agents"),
	Database UMETA(DisplayName = "Database"),
	Network UMETA(DisplayName = "Network"),
	Security UMETA(DisplayName = "Security"),
	Performance UMETA(DisplayName = "Performance"),
	Memory UMETA(DisplayName = "Memory"),
	Concurrency UMETA(DisplayName = "Concurrency"),
	Stress UMETA(DisplayName = "Stress"),
	Integration UMETA(DisplayName = "Integration"),
	XAI UMETA(DisplayName = "XAI"),
	VectorDB UMETA(DisplayName = "Vector Database")
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTestCompleted, const FIGEDTestResult&, Result);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTestSuiteCompleted, const TArray<FIGEDTestResult>&, Results);

UCLASS(Blueprintable)
class IGED_UE5_API UIGEDPressureTester : public UObject
{
	GENERATED_BODY()

public:
	UIGEDPressureTester();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing")
	void Initialize();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing")
	void Shutdown();

	// Test execution
	UFUNCTION(BlueprintCallable, Category = "IGED|Testing")
	void RunAllTests(const FIGEDTestConfig& Config, const FOnTestSuiteCompleted& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing")
	void RunTestCategory(EIGEDTestCategory Category, const FIGEDTestConfig& Config, const FOnTestSuiteCompleted& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing")
	void RunSingleTest(const FString& TestName, const FIGEDTestConfig& Config, const FOnTestCompleted& OnComplete);

	// Core system tests
	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Core")
	FIGEDTestResult TestGameInstanceInitialization();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Core")
	FIGEDTestResult TestOrchestratorFunctionality();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Core")
	FIGEDTestResult TestMemoryEngine();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Core")
	FIGEDTestResult TestEncryptionManager();

	// AI Model tests
	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|AI")
	FIGEDTestResult TestModelManagerInitialization();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|AI")
	FIGEDTestResult TestModelLoading();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|AI")
	FIGEDTestResult TestLLMInference();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|AI")
	FIGEDTestResult TestImageGeneration();

	// Agent tests
	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Agents")
	FIGEDTestResult TestRAGOrchestratorAgent();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Agents")
	FIGEDTestResult TestCodeGenAgent();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Agents")
	FIGEDTestResult TestDataMinerAgent();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Agents")
	FIGEDTestResult TestNetworkIntelligenceAgent();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Agents")
	FIGEDTestResult TestRemoteControlAgent();

	// Database tests
	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Database")
	FIGEDTestResult TestVectorDatabaseInitialization();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Database")
	FIGEDTestResult TestVectorInsertion();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Database")
	FIGEDTestResult TestVectorSearch();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Database")
	FIGEDTestResult TestVectorDatabasePerformance();

	// XAI tests
	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|XAI")
	FIGEDTestResult TestExplainableAIInitialization();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|XAI")
	FIGEDTestResult TestFineTuningProcess();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|XAI")
	FIGEDTestResult TestExplanationGeneration();

	// Network tests
	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Network")
	FIGEDTestResult TestHTTPRequests();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Network")
	FIGEDTestResult TestAPIResponses();

	// Security tests
	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Security")
	FIGEDTestResult TestBiometricAuthentication();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Security")
	FIGEDTestResult TestEncryptionDecryption();

	// Performance tests
	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Performance")
	FIGEDTestResult TestMemoryUsage();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Performance")
	FIGEDTestResult TestCPUUsage();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Performance")
	FIGEDTestResult TestResponseTimes();

	// Stress tests
	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Stress")
	FIGEDTestResult TestHighLoadProcessing();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Stress")
	FIGEDTestResult TestMemoryLeaks();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Stress")
	FIGEDTestResult TestConcurrentOperations();

	// Integration tests
	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Integration")
	FIGEDTestResult TestEndToEndWorkflow();

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing|Integration")
	FIGEDTestResult TestComponentInteraction();

	// Test management
	UFUNCTION(BlueprintPure, Category = "IGED|Testing")
	TArray<FString> GetAvailableTests() const;

	UFUNCTION(BlueprintPure, Category = "IGED|Testing")
	TArray<FString> GetTestsByCategory(EIGEDTestCategory Category) const;

	UFUNCTION(BlueprintCallable, Category = "IGED|Testing")
	void GenerateTestReport(const TArray<FIGEDTestResult>& Results, const FString& OutputPath);

	UFUNCTION(BlueprintPure, Category = "IGED|Testing")
	FString GetSystemHealthSummary() const;

protected:
	UPROPERTY()
	bool bIsInitialized;

	UPROPERTY()
	TMap<EIGEDTestCategory, TArray<FString>> TestCategories;

	UPROPERTY()
	TMap<FString, FIGEDTestResult> TestResults;

	UPROPERTY()
	FIGEDTestConfig CurrentConfig;

private:
	// Helper methods
	void InitializeTestCategories();
	FIGEDTestResult ExecuteTest(const FString& TestName, const FIGEDTestConfig& Config);
	bool ValidateTestResult(const FIGEDTestResult& Result);
	void LogTestResult(const FIGEDTestResult& Result);
	FString FormatTestReport(const TArray<FIGEDTestResult>& Results);
	
	// Test execution helpers
	template<typename TestFunc>
	FIGEDTestResult RunTestWithMetrics(const FString& TestName, TestFunc TestFunction, const FIGEDTestConfig& Config);
	
	// Performance measurement
	float MeasureExecutionTime(TFunction<void()> Function);
	bool CheckMemoryLeaks();
	void LogSystemMetrics();
};

// IGED Advanced Logging System - Comprehensive Error Tracking & Performance Monitoring
// Copyright (c) 2024 Project Human Bot

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Dom/JsonObject.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/Event.h"
#include "Containers/Queue.h"
#include "IGEDLogger.generated.h"

// Log severity levels
UENUM(BlueprintType)
enum class EIGEDLogLevel : uint8
{
	Trace UMETA(DisplayName = "Trace"),
	Debug UMETA(DisplayName = "Debug"),
	Info UMETA(DisplayName = "Info"),
	Warning UMETA(DisplayName = "Warning"),
	Error UMETA(DisplayName = "Error"),
	Critical UMETA(DisplayName = "Critical"),
	Performance UMETA(DisplayName = "Performance"),
	Security UMETA(DisplayName = "Security")
};

// Log categories for better organization
UENUM(BlueprintType)
enum class EIGEDLogCategory : uint8
{
	System UMETA(DisplayName = "System"),
	AI UMETA(DisplayName = "AI"),
	Network UMETA(DisplayName = "Network"),
	Security UMETA(DisplayName = "Security"),
	Performance UMETA(DisplayName = "Performance"),
	User UMETA(DisplayName = "User"),
	Database UMETA(DisplayName = "Database"),
	XAI UMETA(DisplayName = "XAI"),
	VectorDB UMETA(DisplayName = "VectorDB"),
	Agents UMETA(DisplayName = "Agents"),
	Models UMETA(DisplayName = "Models"),
	Voice UMETA(DisplayName = "Voice"),
	Biometric UMETA(DisplayName = "Biometric"),
	Visualization UMETA(DisplayName = "Visualization")
};

// Log entry structure
USTRUCT(BlueprintType)
struct FIGEDLogEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	FDateTime Timestamp;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	EIGEDLogLevel Level;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	EIGEDLogCategory Category;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	FString Message;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	FString FunctionName;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	FString FileName;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	int32 LineNumber;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	FString ThreadID;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	TMap<FString, FString> AdditionalData;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	float ExecutionTime;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	FString StackTrace;

	FIGEDLogEntry()
		: Timestamp(FDateTime::Now())
		, Level(EIGEDLogLevel::Info)
		, Category(EIGEDLogCategory::System)
		, LineNumber(0)
		, ExecutionTime(0.0f)
	{}
};

// Performance metrics structure
USTRUCT(BlueprintType)
struct FIGEDPerformanceMetrics
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	FString FunctionName;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	float AverageExecutionTime;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	float MinExecutionTime;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	float MaxExecutionTime;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	int32 CallCount;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	int32 ErrorCount;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	float SuccessRate;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	FDateTime LastCalled;

	FIGEDPerformanceMetrics()
		: AverageExecutionTime(0.0f)
		, MinExecutionTime(FLT_MAX)
		, MaxExecutionTime(0.0f)
		, CallCount(0)
		, ErrorCount(0)
		, SuccessRate(100.0f)
	{}
};

// Logger configuration
USTRUCT(BlueprintType)
struct FIGEDLoggerConfig
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	FString LogDirectory;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	EIGEDLogLevel MinLogLevel;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	bool bEnableFileLogging;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	bool bEnableConsoleLogging;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	bool bEnablePerformanceLogging;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	bool bEnableStackTrace;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	int32 MaxLogFileSize;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	int32 MaxLogFiles;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	bool bEnableAsyncLogging;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	bool bEnableMetricsCollection;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Logger")
	float MetricsUpdateInterval;

	FIGEDLoggerConfig()
		: LogDirectory(TEXT("Logs"))
		, MinLogLevel(EIGEDLogLevel::Debug)
		, bEnableFileLogging(true)
		, bEnableConsoleLogging(true)
		, bEnablePerformanceLogging(true)
		, bEnableStackTrace(true)
		, MaxLogFileSize(10485760) // 10MB
		, MaxLogFiles(10)
		, bEnableAsyncLogging(true)
		, bEnableMetricsCollection(true)
		, MetricsUpdateInterval(5.0f)
	{}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLogEntryAdded, const FIGEDLogEntry&, LogEntry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerformanceMetricsUpdated, const TMap<FString, FIGEDPerformanceMetrics>&, Metrics);

// Async logging thread
class FIGEDLoggingThread : public FRunnable
{
public:
	FIGEDLoggingThread();
	virtual ~FIGEDLoggingThread();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

	void AddLogEntry(const FIGEDLogEntry& Entry);
	void SetConfig(const FIGEDLoggerConfig& Config);

private:
	FRunnableThread* Thread;
	FEvent* WakeUpEvent;
	FCriticalSection QueueMutex;
	TQueue<FIGEDLogEntry> LogQueue;
	FIGEDLoggerConfig Config;
	bool bShouldStop;

	void ProcessLogEntry(const FIGEDLogEntry& Entry);
	void WriteToFile(const FIGEDLogEntry& Entry);
	void WriteToConsole(const FIGEDLogEntry& Entry);
	FString FormatLogEntry(const FIGEDLogEntry& Entry);
};

UCLASS(Blueprintable)
class IGED_UE5_API UIGEDLogger : public UObject
{
	GENERATED_BODY()

public:
	UIGEDLogger();

	UFUNCTION(BlueprintCallable, Category = "IGED|Logger")
	void Initialize(const FIGEDLoggerConfig& Config);

	UFUNCTION(BlueprintCallable, Category = "IGED|Logger")
	void Shutdown();

	// Core logging functions
	UFUNCTION(BlueprintCallable, Category = "IGED|Logger")
	void Log(EIGEDLogLevel Level, EIGEDLogCategory Category, const FString& Message, const FString& FunctionName = TEXT(""), const FString& FileName = TEXT(""), int32 LineNumber = 0);

	UFUNCTION(BlueprintCallable, Category = "IGED|Logger")
	void LogTrace(EIGEDLogCategory Category, const FString& Message, const FString& FunctionName = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "IGED|Logger")
	void LogDebug(EIGEDLogCategory Category, const FString& Message, const FString& FunctionName = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "IGED|Logger")
	void LogInfo(EIGEDLogCategory Category, const FString& Message, const FString& FunctionName = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "IGED|Logger")
	void LogWarning(EIGEDLogCategory Category, const FString& Message, const FString& FunctionName = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "IGED|Logger")
	void LogError(EIGEDLogCategory Category, const FString& Message, const FString& FunctionName = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "IGED|Logger")
	void LogCritical(EIGEDLogCategory Category, const FString& Message, const FString& FunctionName = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "IGED|Logger")
	void LogPerformance(EIGEDLogCategory Category, const FString& Message, float ExecutionTime, const FString& FunctionName = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "IGED|Logger")
	void LogSecurity(EIGEDLogCategory Category, const FString& Message, const FString& FunctionName = TEXT(""));

	// Performance monitoring
	UFUNCTION(BlueprintCallable, Category = "IGED|Logger")
	void StartPerformanceTimer(const FString& FunctionName);

	UFUNCTION(BlueprintCallable, Category = "IGED|Logger")
	void EndPerformanceTimer(const FString& FunctionName, bool bSuccess = true);

	UFUNCTION(BlueprintCallable, Category = "IGED|Logger")
	void RecordFunctionCall(const FString& FunctionName, float ExecutionTime, bool bSuccess);

	// Metrics and statistics
	UFUNCTION(BlueprintPure, Category = "IGED|Logger")
	TMap<FString, FIGEDPerformanceMetrics> GetPerformanceMetrics() const;

	UFUNCTION(BlueprintPure, Category = "IGED|Logger")
	FString GetSystemHealthReport() const;

	UFUNCTION(BlueprintPure, Category = "IGED|Logger")
	int32 GetErrorCount() const;

	UFUNCTION(BlueprintPure, Category = "IGED|Logger")
	int32 GetWarningCount() const;

	UFUNCTION(BlueprintPure, Category = "IGED|Logger")
	float GetAveragePerformance() const;

	// Log management
	UFUNCTION(BlueprintCallable, Category = "IGED|Logger")
	void ClearLogs();

	UFUNCTION(BlueprintCallable, Category = "IGED|Logger")
	void ExportLogs(const FString& ExportPath);

	UFUNCTION(BlueprintCallable, Category = "IGED|Logger")
	void SetLogLevel(EIGEDLogLevel NewLevel);

	UFUNCTION(BlueprintCallable, Category = "IGED|Logger")
	void SetCategoryEnabled(EIGEDLogCategory Category, bool bEnabled);

	// Events
	UPROPERTY(BlueprintAssignable, Category = "IGED|Logger|Events")
	FOnLogEntryAdded OnLogEntryAdded;

	UPROPERTY(BlueprintAssignable, Category = "IGED|Logger|Events")
	FOnPerformanceMetricsUpdated OnPerformanceMetricsUpdated;

protected:
	UPROPERTY()
	bool bIsInitialized;

	UPROPERTY()
	FIGEDLoggerConfig Config;

	UPROPERTY()
	TMap<FString, FIGEDPerformanceMetrics> PerformanceMetrics;

	UPROPERTY()
	TMap<EIGEDLogCategory, bool> CategoryEnabled;

	UPROPERTY()
	TMap<FString, FDateTime> PerformanceTimers;

	UPROPERTY()
	int32 ErrorCount;

	UPROPERTY()
	int32 WarningCount;

	UPROPERTY()
	int32 TotalLogCount;

private:
	FIGEDLoggingThread* LoggingThread;
	FCriticalSection MetricsMutex;
	FTimerHandle MetricsUpdateTimer;

	void UpdatePerformanceMetrics();
	void CleanupOldLogFiles();
	FString GetLogFileName() const;
	FString GetStackTrace() const;
	void InitializeCategories();
};

// Convenience macros for logging
#define IGED_LOG_TRACE(Category, Message) \
	UIGEDLogger::GetInstance()->LogTrace(Category, Message, TEXT(__FUNCTION__))

#define IGED_LOG_DEBUG(Category, Message) \
	UIGEDLogger::GetInstance()->LogDebug(Category, Message, TEXT(__FUNCTION__))

#define IGED_LOG_INFO(Category, Message) \
	UIGEDLogger::GetInstance()->LogInfo(Category, Message, TEXT(__FUNCTION__))

#define IGED_LOG_WARNING(Category, Message) \
	UIGEDLogger::GetInstance()->LogWarning(Category, Message, TEXT(__FUNCTION__))

#define IGED_LOG_ERROR(Category, Message) \
	UIGEDLogger::GetInstance()->LogError(Category, Message, TEXT(__FUNCTION__))

#define IGED_LOG_CRITICAL(Category, Message) \
	UIGEDLogger::GetInstance()->LogCritical(Category, Message, TEXT(__FUNCTION__))

#define IGED_LOG_PERFORMANCE(Category, Message, Time) \
	UIGEDLogger::GetInstance()->LogPerformance(Category, Message, Time, TEXT(__FUNCTION__))

#define IGED_LOG_SECURITY(Category, Message) \
	UIGEDLogger::GetInstance()->LogSecurity(Category, Message, TEXT(__FUNCTION__))

#define IGED_PERFORMANCE_TIMER(FunctionName) \
	UIGEDLogger::GetInstance()->StartPerformanceTimer(FunctionName)

#define IGED_END_PERFORMANCE_TIMER(FunctionName, Success) \
	UIGEDLogger::GetInstance()->EndPerformanceTimer(FunctionName, Success)

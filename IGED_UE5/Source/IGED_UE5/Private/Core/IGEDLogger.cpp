// IGED Advanced Logging System - Comprehensive Error Tracking & Performance Monitoring Implementation
// Copyright (c) 2024 Project Human Bot

#include "Core/IGEDLogger.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Serialization/JsonSerializer.h"
#include "Engine/Engine.h"
#include "HAL/PlatformMisc.h"
#include "Misc/DateTime.h"
#include "Misc/FileHelper.h"

// Static instance for global access
static UIGEDLogger* GIGEDLoggerInstance = nullptr;

// Logging Thread Implementation
FIGEDLoggingThread::FIGEDLoggingThread()
	: Thread(nullptr)
	, WakeUpEvent(nullptr)
	, bShouldStop(false)
{
	WakeUpEvent = FPlatformProcess::GetSynchEventFromPool();
}

FIGEDLoggingThread::~FIGEDLoggingThread()
{
	Stop();
	if (WakeUpEvent)
	{
		FPlatformProcess::ReturnSynchEventToPool(WakeUpEvent);
		WakeUpEvent = nullptr;
	}
}

bool FIGEDLoggingThread::Init()
{
	return true;
}

uint32 FIGEDLoggingThread::Run()
{
	while (!bShouldStop)
	{
		// Wait for wake up event or timeout
		WakeUpEvent->Wait(100); // 100ms timeout

		// Process all queued log entries
		TArray<FIGEDLogEntry> EntriesToProcess;
		{
			FScopeLock Lock(&QueueMutex);
			while (!LogQueue.IsEmpty())
			{
				FIGEDLogEntry Entry;
				if (LogQueue.Dequeue(Entry))
				{
					EntriesToProcess.Add(Entry);
				}
			}
		}

		// Process entries
		for (const FIGEDLogEntry& Entry : EntriesToProcess)
		{
			ProcessLogEntry(Entry);
		}
	}

	return 0;
}

void FIGEDLoggingThread::Stop()
{
	bShouldStop = true;
	if (WakeUpEvent)
	{
		WakeUpEvent->Trigger();
	}
	if (Thread)
	{
		Thread->WaitForCompletion();
		delete Thread;
		Thread = nullptr;
	}
}

void FIGEDLoggingThread::Exit()
{
	// Cleanup
}

void FIGEDLoggingThread::AddLogEntry(const FIGEDLogEntry& Entry)
{
	{
		FScopeLock Lock(&QueueMutex);
		LogQueue.Enqueue(Entry);
	}
	if (WakeUpEvent)
	{
		WakeUpEvent->Trigger();
	}
}

void FIGEDLoggingThread::SetConfig(const FIGEDLoggerConfig& NewConfig)
{
	Config = NewConfig;
}

void FIGEDLoggingThread::ProcessLogEntry(const FIGEDLogEntry& Entry)
{
	if (Config.bEnableFileLogging)
	{
		WriteToFile(Entry);
	}
	if (Config.bEnableConsoleLogging)
	{
		WriteToConsole(Entry);
	}
}

void FIGEDLoggingThread::WriteToFile(const FIGEDLogEntry& Entry)
{
	FString LogFileName = FPaths::Combine(Config.LogDirectory, FString::Printf(TEXT("IGED_%s.log"), *FDateTime::Now().ToString(TEXT("%Y%m%d"))));
	
	FString LogLine = FormatLogEntry(Entry) + TEXT("\n");
	
	FFileHelper::SaveStringToFile(LogLine, *LogFileName, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
}

void FIGEDLoggingThread::WriteToConsole(const FIGEDLogEntry& Entry)
{
	FString LogLine = FormatLogEntry(Entry);
	
	// Use appropriate UE log level
	switch (Entry.Level)
	{
	case EIGEDLogLevel::Trace:
	case EIGEDLogLevel::Debug:
		UE_LOG(LogTemp, VeryVerbose, TEXT("%s"), *LogLine);
		break;
	case EIGEDLogLevel::Info:
		UE_LOG(LogTemp, Log, TEXT("%s"), *LogLine);
		break;
	case EIGEDLogLevel::Warning:
		UE_LOG(LogTemp, Warning, TEXT("%s"), *LogLine);
		break;
	case EIGEDLogLevel::Error:
		UE_LOG(LogTemp, Error, TEXT("%s"), *LogLine);
		break;
	case EIGEDLogLevel::Critical:
		UE_LOG(LogTemp, Fatal, TEXT("%s"), *LogLine);
		break;
	case EIGEDLogLevel::Performance:
		UE_LOG(LogTemp, Log, TEXT("[PERF] %s"), *LogLine);
		break;
	case EIGEDLogLevel::Security:
		UE_LOG(LogTemp, Warning, TEXT("[SEC] %s"), *LogLine);
		break;
	}
}

FString FIGEDLoggingThread::FormatLogEntry(const FIGEDLogEntry& Entry)
{
	FString LevelString;
	switch (Entry.Level)
	{
	case EIGEDLogLevel::Trace: LevelString = TEXT("TRACE"); break;
	case EIGEDLogLevel::Debug: LevelString = TEXT("DEBUG"); break;
	case EIGEDLogLevel::Info: LevelString = TEXT("INFO"); break;
	case EIGEDLogLevel::Warning: LevelString = TEXT("WARN"); break;
	case EIGEDLogLevel::Error: LevelString = TEXT("ERROR"); break;
	case EIGEDLogLevel::Critical: LevelString = TEXT("CRITICAL"); break;
	case EIGEDLogLevel::Performance: LevelString = TEXT("PERF"); break;
	case EIGEDLogLevel::Security: LevelString = TEXT("SEC"); break;
	}

	FString CategoryString;
	switch (Entry.Category)
	{
	case EIGEDLogCategory::System: CategoryString = TEXT("SYS"); break;
	case EIGEDLogCategory::AI: CategoryString = TEXT("AI"); break;
	case EIGEDLogCategory::Network: CategoryString = TEXT("NET"); break;
	case EIGEDLogCategory::Security: CategoryString = TEXT("SEC"); break;
	case EIGEDLogCategory::Performance: CategoryString = TEXT("PERF"); break;
	case EIGEDLogCategory::User: CategoryString = TEXT("USER"); break;
	case EIGEDLogCategory::Database: CategoryString = TEXT("DB"); break;
	case EIGEDLogCategory::XAI: CategoryString = TEXT("XAI"); break;
	case EIGEDLogCategory::VectorDB: CategoryString = TEXT("VDB"); break;
	case EIGEDLogCategory::Agents: CategoryString = TEXT("AGENT"); break;
	case EIGEDLogCategory::Models: CategoryString = TEXT("MODEL"); break;
	case EIGEDLogCategory::Voice: CategoryString = TEXT("VOICE"); break;
	case EIGEDLogCategory::Biometric: CategoryString = TEXT("BIO"); break;
	case EIGEDLogCategory::Visualization: CategoryString = TEXT("VIZ"); break;
	}

	FString FormattedEntry = FString::Printf(TEXT("[%s] [%s] [%s] [%s] %s"),
		*Entry.Timestamp.ToString(TEXT("%Y-%m-%d %H:%M:%S.%f")),
		*LevelString,
		*CategoryString,
		*Entry.ThreadID,
		*Entry.Message
	);

	if (!Entry.FunctionName.IsEmpty())
	{
		FormattedEntry += FString::Printf(TEXT(" | Function: %s"), *Entry.FunctionName);
	}

	if (Entry.LineNumber > 0)
	{
		FormattedEntry += FString::Printf(TEXT(" | Line: %d"), Entry.LineNumber);
	}

	if (Entry.ExecutionTime > 0.0f)
	{
		FormattedEntry += FString::Printf(TEXT(" | Time: %.3fms"), Entry.ExecutionTime);
	}

	if (!Entry.AdditionalData.IsEmpty())
	{
		FormattedEntry += TEXT(" | Data: ");
		for (const auto& Data : Entry.AdditionalData)
		{
			FormattedEntry += FString::Printf(TEXT("%s=%s; "), *Data.Key, *Data.Value);
		}
	}

	return FormattedEntry;
}

// Logger Implementation
UIGEDLogger::UIGEDLogger()
	: bIsInitialized(false)
	, LoggingThread(nullptr)
	, ErrorCount(0)
	, WarningCount(0)
	, TotalLogCount(0)
{
	GIGEDLoggerInstance = this;
}

void UIGEDLogger::Initialize(const FIGEDLoggerConfig& NewConfig)
{
	if (bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("IGEDLogger already initialized."));
		return;
	}

	Config = NewConfig;
	
	// Create log directory
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*Config.LogDirectory))
	{
		PlatformFile.CreateDirectoryTree(*Config.LogDirectory);
	}

	// Initialize categories
	InitializeCategories();

	// Start async logging thread if enabled
	if (Config.bEnableAsyncLogging)
	{
		LoggingThread = new FIGEDLoggingThread();
		LoggingThread->SetConfig(Config);
		LoggingThread->Thread = FRunnableThread::Create(LoggingThread, TEXT("IGEDLoggingThread"));
	}

	// Start metrics update timer
	if (Config.bEnableMetricsCollection && GEngine && GEngine->GetWorld())
	{
		GEngine->GetWorld()->GetTimerManager().SetTimer(MetricsUpdateTimer, this, &UIGEDLogger::UpdatePerformanceMetrics, Config.MetricsUpdateInterval, true);
	}

	bIsInitialized = true;
	
	// Log initialization
	LogInfo(EIGEDLogCategory::System, TEXT("IGEDLogger initialized successfully"));
}

void UIGEDLogger::Shutdown()
{
	if (!bIsInitialized)
	{
		return;
	}

	// Stop metrics timer
	if (MetricsUpdateTimer.IsValid() && GEngine && GEngine->GetWorld())
	{
		GEngine->GetWorld()->GetTimerManager().ClearTimer(MetricsUpdateTimer);
	}

	// Stop logging thread
	if (LoggingThread)
	{
		LoggingThread->Stop();
		delete LoggingThread;
		LoggingThread = nullptr;
	}

	// Cleanup old log files
	CleanupOldLogFiles();

	bIsInitialized = false;
	GIGEDLoggerInstance = nullptr;
}

void UIGEDLogger::Log(EIGEDLogLevel Level, EIGEDLogCategory Category, const FString& Message, const FString& FunctionName, const FString& FileName, int32 LineNumber)
{
	if (!bIsInitialized || Level < Config.MinLogLevel || !CategoryEnabled[Category])
	{
		return;
	}

	FIGEDLogEntry Entry;
	Entry.Level = Level;
	Entry.Category = Category;
	Entry.Message = Message;
	Entry.FunctionName = FunctionName;
	Entry.FileName = FileName;
	Entry.LineNumber = LineNumber;
	Entry.ThreadID = FString::Printf(TEXT("%d"), FPlatformTLS::GetCurrentThreadId());
	
	if (Config.bEnableStackTrace && (Level >= EIGEDLogLevel::Error))
	{
		Entry.StackTrace = GetStackTrace();
	}

	// Update counters
	TotalLogCount++;
	if (Level == EIGEDLogLevel::Error || Level == EIGEDLogLevel::Critical)
	{
		ErrorCount++;
	}
	else if (Level == EIGEDLogLevel::Warning)
	{
		WarningCount++;
	}

	// Add to async queue or process immediately
	if (Config.bEnableAsyncLogging && LoggingThread)
	{
		LoggingThread->AddLogEntry(Entry);
	}
	else
	{
		// Process immediately
		if (Config.bEnableFileLogging)
		{
			FString LogFileName = GetLogFileName();
			FString LogLine = FString::Printf(TEXT("[%s] [%s] [%s] %s\n"),
				*Entry.Timestamp.ToString(TEXT("%Y-%m-%d %H:%M:%S.%f")),
				*FString::Printf(TEXT("%d"), (int32)Level),
				*FString::Printf(TEXT("%d"), (int32)Category),
				*Message
			);
			FFileHelper::SaveStringToFile(LogLine, *LogFileName, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
		}
	}

	// Broadcast event
	OnLogEntryAdded.Broadcast(Entry);
}

void UIGEDLogger::LogTrace(EIGEDLogCategory Category, const FString& Message, const FString& FunctionName)
{
	Log(EIGEDLogLevel::Trace, Category, Message, FunctionName);
}

void UIGEDLogger::LogDebug(EIGEDLogCategory Category, const FString& Message, const FString& FunctionName)
{
	Log(EIGEDLogLevel::Debug, Category, Message, FunctionName);
}

void UIGEDLogger::LogInfo(EIGEDLogCategory Category, const FString& Message, const FString& FunctionName)
{
	Log(EIGEDLogLevel::Info, Category, Message, FunctionName);
}

void UIGEDLogger::LogWarning(EIGEDLogCategory Category, const FString& Message, const FString& FunctionName)
{
	Log(EIGEDLogLevel::Warning, Category, Message, FunctionName);
}

void UIGEDLogger::LogError(EIGEDLogCategory Category, const FString& Message, const FString& FunctionName)
{
	Log(EIGEDLogLevel::Error, Category, Message, FunctionName);
}

void UIGEDLogger::LogCritical(EIGEDLogCategory Category, const FString& Message, const FString& FunctionName)
{
	Log(EIGEDLogLevel::Critical, Category, Message, FunctionName);
}

void UIGEDLogger::LogPerformance(EIGEDLogCategory Category, const FString& Message, float ExecutionTime, const FString& FunctionName)
{
	FIGEDLogEntry Entry;
	Entry.Level = EIGEDLogLevel::Performance;
	Entry.Category = Category;
	Entry.Message = Message;
	Entry.FunctionName = FunctionName;
	Entry.ExecutionTime = ExecutionTime;
	Entry.ThreadID = FString::Printf(TEXT("%d"), FPlatformTLS::GetCurrentThreadId());

	// Record performance metrics
	RecordFunctionCall(FunctionName, ExecutionTime, true);

	// Add to async queue or process immediately
	if (Config.bEnableAsyncLogging && LoggingThread)
	{
		LoggingThread->AddLogEntry(Entry);
	}

	OnLogEntryAdded.Broadcast(Entry);
}

void UIGEDLogger::LogSecurity(EIGEDLogCategory Category, const FString& Message, const FString& FunctionName)
{
	Log(EIGEDLogLevel::Security, Category, Message, FunctionName);
}

void UIGEDLogger::StartPerformanceTimer(const FString& FunctionName)
{
	PerformanceTimers.Add(FunctionName, FDateTime::Now());
}

void UIGEDLogger::EndPerformanceTimer(const FString& FunctionName, bool bSuccess)
{
	if (FDateTime* StartTime = PerformanceTimers.Find(FunctionName))
	{
		float ExecutionTime = (FDateTime::Now() - *StartTime).GetTotalMilliseconds();
		RecordFunctionCall(FunctionName, ExecutionTime, bSuccess);
		PerformanceTimers.Remove(FunctionName);
	}
}

void UIGEDLogger::RecordFunctionCall(const FString& FunctionName, float ExecutionTime, bool bSuccess)
{
	FScopeLock Lock(&MetricsMutex);
	
	FIGEDPerformanceMetrics& Metrics = PerformanceMetrics.FindOrAdd(FunctionName);
	Metrics.FunctionName = FunctionName;
	Metrics.CallCount++;
	Metrics.LastCalled = FDateTime::Now();
	
	if (bSuccess)
	{
		Metrics.AverageExecutionTime = (Metrics.AverageExecutionTime * (Metrics.CallCount - 1) + ExecutionTime) / Metrics.CallCount;
		Metrics.MinExecutionTime = FMath::Min(Metrics.MinExecutionTime, ExecutionTime);
		Metrics.MaxExecutionTime = FMath::Max(Metrics.MaxExecutionTime, ExecutionTime);
	}
	else
	{
		Metrics.ErrorCount++;
	}
	
	Metrics.SuccessRate = ((float)(Metrics.CallCount - Metrics.ErrorCount) / Metrics.CallCount) * 100.0f;
}

TMap<FString, FIGEDPerformanceMetrics> UIGEDLogger::GetPerformanceMetrics() const
{
	FScopeLock Lock(&MetricsMutex);
	return PerformanceMetrics;
}

FString UIGEDLogger::GetSystemHealthReport() const
{
	FString Report = TEXT("=== IGED System Health Report ===\n");
	Report += FString::Printf(TEXT("Total Logs: %d\n"), TotalLogCount);
	Report += FString::Printf(TEXT("Errors: %d\n"), ErrorCount);
	Report += FString::Printf(TEXT("Warnings: %d\n"), WarningCount);
	Report += FString::Printf(TEXT("Error Rate: %.2f%%\n"), TotalLogCount > 0 ? (float)ErrorCount / TotalLogCount * 100.0f : 0.0f);
	
	FScopeLock Lock(&MetricsMutex);
	Report += TEXT("\n=== Performance Metrics ===\n");
	for (const auto& Metric : PerformanceMetrics)
	{
		const FIGEDPerformanceMetrics& Perf = Metric.Value;
		Report += FString::Printf(TEXT("%s: Calls=%d, AvgTime=%.3fms, Success=%.1f%%\n"),
			*Perf.FunctionName, Perf.CallCount, Perf.AverageExecutionTime, Perf.SuccessRate);
	}
	
	return Report;
}

int32 UIGEDLogger::GetErrorCount() const
{
	return ErrorCount;
}

int32 UIGEDLogger::GetWarningCount() const
{
	return WarningCount;
}

float UIGEDLogger::GetAveragePerformance() const
{
	FScopeLock Lock(&MetricsMutex);
	if (PerformanceMetrics.IsEmpty())
	{
		return 0.0f;
	}
	
	float TotalAvg = 0.0f;
	for (const auto& Metric : PerformanceMetrics)
	{
		TotalAvg += Metric.Value.AverageExecutionTime;
	}
	
	return TotalAvg / PerformanceMetrics.Num();
}

void UIGEDLogger::ClearLogs()
{
	// Clear performance metrics
	{
		FScopeLock Lock(&MetricsMutex);
		PerformanceMetrics.Empty();
	}
	
	// Reset counters
	ErrorCount = 0;
	WarningCount = 0;
	TotalLogCount = 0;
	
	LogInfo(EIGEDLogCategory::System, TEXT("Logs cleared"));
}

void UIGEDLogger::ExportLogs(const FString& ExportPath)
{
	FString Report = GetSystemHealthReport();
	FFileHelper::SaveStringToFile(Report, *ExportPath);
	LogInfo(EIGEDLogCategory::System, FString::Printf(TEXT("Logs exported to: %s"), *ExportPath));
}

void UIGEDLogger::SetLogLevel(EIGEDLogLevel NewLevel)
{
	Config.MinLogLevel = NewLevel;
	LogInfo(EIGEDLogCategory::System, FString::Printf(TEXT("Log level set to: %d"), (int32)NewLevel));
}

void UIGEDLogger::SetCategoryEnabled(EIGEDLogCategory Category, bool bEnabled)
{
	CategoryEnabled[Category] = bEnabled;
	LogInfo(EIGEDLogCategory::System, FString::Printf(TEXT("Category %d %s"), (int32)Category, bEnabled ? TEXT("enabled") : TEXT("disabled")));
}

void UIGEDLogger::UpdatePerformanceMetrics()
{
	OnPerformanceMetricsUpdated.Broadcast(GetPerformanceMetrics());
}

void UIGEDLogger::CleanupOldLogFiles()
{
	// Implementation for cleaning up old log files
	// This would check file sizes and dates, and remove old files
}

FString UIGEDLogger::GetLogFileName() const
{
	return FPaths::Combine(Config.LogDirectory, FString::Printf(TEXT("IGED_%s.log"), *FDateTime::Now().ToString(TEXT("%Y%m%d"))));
}

FString UIGEDLogger::GetStackTrace() const
{
	// Simple stack trace implementation
	return TEXT("Stack trace not implemented");
}

void UIGEDLogger::InitializeCategories()
{
	// Enable all categories by default
	for (int32 i = 0; i < (int32)EIGEDLogCategory::Visualization + 1; i++)
	{
		CategoryEnabled.Add((EIGEDLogCategory)i, true);
	}
}

// Global access function
UIGEDLogger* UIGEDLogger::GetInstance()
{
	return GIGEDLoggerInstance;
}

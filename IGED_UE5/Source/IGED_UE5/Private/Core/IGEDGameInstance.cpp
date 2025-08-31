// IGED Game Instance - Core Application Manager Implementation
// Copyright (c) 2024 Project Human Bot

#include "Core/IGEDGameInstance.h"
#include "Core/IGEDLogger.h"
#include "Core/IGEDPressureTester.h"
#include "Core/IGEDSystemValidator.h"
#include "Core/IGEDOrchestrator.h"
#include "Core/IGEDModelManager.h"
#include "Core/IGEDExplainableAI.h"
#include "Core/IGEDVectorDatabase.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Serialization/JsonSerializer.h"
#include "Engine/Engine.h"

UIGEDGameInstance::UIGEDGameInstance()
	: CurrentSystemState(EIGEDSystemState::Uninitialized)
	, SystemVersion(TEXT("1.0.0"))
	, bOfflineMode(false)
	, bDebugMode(true)
	, VoiceSensitivity(0.5f)
	, HttpModule(nullptr)
	, CurrentFPS(0.0f)
	, MemoryUsageMB(0.0f)
	, ActiveThreadCount(0)
{
	// Initialize GameInstance
}

void UIGEDGameInstance::Init()
{
	Super::Init();
	
	// Initialize logging system first
	InitializeLoggingSystem();
	
	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("IGED GameInstance initializing"));
	
	// Initialize core components
	InitializeComponents();
	
	// Initialize visualization
	InitializeVisualization();
	
	// Initialize networking
	InitializeNetworking();
	
	// Set initial state
	SetSystemState(EIGEDSystemState::Initializing);
	
	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("IGED GameInstance initialized successfully"));
}

void UIGEDGameInstance::Shutdown()
{
	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("IGED GameInstance shutting down"));
	
	SetSystemState(EIGEDSystemState::ShuttingDown);
	
	// Shutdown components
	ShutdownSystem();
	
	Super::Shutdown();
	
	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("IGED GameInstance shut down"));
}

void UIGEDGameInstance::StartGameInstance()
{
	Super::StartGameInstance();
	
	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("IGED GameInstance started"));
	
	// Run comprehensive tests
	RunComprehensiveTests();
}

void UIGEDGameInstance::InitializeSystem()
{
	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("Initializing IGED system components"));
	
	// Initialize all core components
	if (!Orchestrator)
	{
		Orchestrator = NewObject<UIGEDOrchestrator>(this);
		if (Orchestrator)
		{
			IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("Orchestrator created"));
		}
		else
		{
			IGED_LOG_ERROR(EIGEDLogCategory::System, TEXT("Failed to create Orchestrator"));
		}
	}
	
	if (!ModelManager)
	{
		ModelManager = NewObject<UIGEDModelManager>(this);
		if (ModelManager)
		{
			ModelManager->Initialize();
			IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("ModelManager initialized"));
		}
		else
		{
			IGED_LOG_ERROR(EIGEDLogCategory::System, TEXT("Failed to create ModelManager"));
		}
	}
	
	if (!ExplainableAI)
	{
		ExplainableAI = NewObject<UIGEDExplainableAI>(this);
		if (ExplainableAI)
		{
			ExplainableAI->Initialize();
			IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("ExplainableAI initialized"));
		}
		else
		{
			IGED_LOG_ERROR(EIGEDLogCategory::System, TEXT("Failed to create ExplainableAI"));
		}
	}
	
	if (!VectorDatabase)
	{
		VectorDatabase = NewObject<UIGEDVectorDatabase>(this);
		if (VectorDatabase)
		{
			FVectorDatabaseConfig Config;
			Config.DatabaseName = TEXT("IGED_KnowledgeBase");
			Config.StoragePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("VectorDB"));
			Config.EmbeddingDimensions = 768;
			Config.MaxResults = 10;
			Config.SimilarityThreshold = 0.7f;
			Config.bUseIndexing = true;
			Config.IndexUpdateInterval = 100;
			
			VectorDatabase->Initialize(Config);
			IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("VectorDatabase initialized"));
		}
		else
		{
			IGED_LOG_ERROR(EIGEDLogCategory::System, TEXT("Failed to create VectorDatabase"));
		}
	}
	
	// Initialize orchestrator with dependencies
	if (Orchestrator && ModelManager)
	{
		Orchestrator->Initialize(MemoryEngine, ModelManager);
		IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("Orchestrator initialized with dependencies"));
	}
	
	SetSystemState(EIGEDSystemState::Ready);
	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("IGED system initialization completed"));
}

void UIGEDGameInstance::ShutdownSystem()
{
	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("Shutting down IGED system components"));
	
	// Shutdown components in reverse order
	if (VectorDatabase)
	{
		VectorDatabase->Shutdown();
		VectorDatabase = nullptr;
	}
	
	if (ExplainableAI)
	{
		ExplainableAI->Shutdown();
		ExplainableAI = nullptr;
	}
	
	if (ModelManager)
	{
		ModelManager->Shutdown();
		ModelManager = nullptr;
	}
	
	if (Orchestrator)
	{
		Orchestrator = nullptr;
	}
	
	SetSystemState(EIGEDSystemState::Uninitialized);
	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("IGED system shutdown completed"));
}

bool UIGEDGameInstance::ExecuteCommand(const FString& Command)
{
	if (!Orchestrator)
	{
		IGED_LOG_ERROR(EIGEDLogCategory::System, TEXT("Orchestrator not available for command execution"));
		return false;
	}
	
	IGED_LOG_INFO(EIGEDLogCategory::System, FString::Printf(TEXT("Executing command: %s"), *Command));
	
	bool bResult = Orchestrator->ExecuteCommand(Command);
	
	OnCommandExecuted.Broadcast(Command, bResult ? TEXT("Success") : TEXT("Failed"));
	
	return bResult;
}

FString UIGEDGameInstance::GetSystemStatus() const
{
	FString Status = FString::Printf(TEXT("IGED System v%s - State: %d"), *SystemVersion, (int32)CurrentSystemState);
	
	if (Orchestrator)
	{
		TArray<FString> AvailableAgents = Orchestrator->GetAvailableAgents();
		Status += FString::Printf(TEXT(" - Agents: %d"), AvailableAgents.Num());
	}
	
	if (ModelManager)
	{
		Status += FString::Printf(TEXT(" - Active Model: %s"), *ModelManager->GetActiveModelName());
	}
	
	if (VectorDatabase)
	{
		Status += FString::Printf(TEXT(" - Vectors: %d"), VectorDatabase->GetVectorCount());
	}
	
	return Status;
}

void UIGEDGameInstance::SetSystemState(EIGEDSystemState NewState)
{
	if (CurrentSystemState != NewState)
	{
		CurrentSystemState = NewState;
		FString StateString = FString::Printf(TEXT("%d"), (int32)NewState);
		OnSystemStatusChanged.Broadcast(StateString);
		
		IGED_LOG_INFO(EIGEDLogCategory::System, FString::Printf(TEXT("System state changed to: %d"), (int32)NewState));
	}
}

void UIGEDGameInstance::EnableAgent(const FString& AgentName)
{
	if (Orchestrator)
	{
		// Implementation for enabling specific agent
		IGED_LOG_INFO(EIGEDLogCategory::Agents, FString::Printf(TEXT("Enabling agent: %s"), *AgentName));
		OnAgentStateChanged.Broadcast(AgentName, true);
	}
}

void UIGEDGameInstance::DisableAgent(const FString& AgentName)
{
	if (Orchestrator)
	{
		// Implementation for disabling specific agent
		IGED_LOG_INFO(EIGEDLogCategory::Agents, FString::Printf(TEXT("Disabling agent: %s"), *AgentName));
		OnAgentStateChanged.Broadcast(AgentName, false);
	}
}

TArray<FString> UIGEDGameInstance::GetAvailableAgents() const
{
	if (Orchestrator)
	{
		return Orchestrator->GetAvailableAgents();
	}
	return TArray<FString>();
}

void UIGEDGameInstance::StartVoiceListening()
{
	IGED_LOG_INFO(EIGEDLogCategory::Voice, TEXT("Starting voice listening"));
	// Implementation for voice listening
}

void UIGEDGameInstance::StopVoiceListening()
{
	IGED_LOG_INFO(EIGEDLogCategory::Voice, TEXT("Stopping voice listening"));
	// Implementation for stopping voice listening
}

bool UIGEDGameInstance::IsVoiceListening() const
{
	// Implementation for checking voice listening status
	return false;
}

void UIGEDGameInstance::SendHTTPRequest(const FString& URL, const FString& Verb, const FString& Content)
{
	if (!HttpModule)
	{
		HttpModule = &FHttpModule::Get();
	}
	
	IGED_LOG_INFO(EIGEDLogCategory::Network, FString::Printf(TEXT("Sending HTTP %s request to: %s"), *Verb, *URL));
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule->CreateRequest();
	Request->SetURL(URL);
	Request->SetVerb(Verb);
	Request->SetContentAsString(Content);
	
	Request->OnProcessRequestComplete().BindUObject(this, &UIGEDGameInstance::OnHttpResponseReceived);
	Request->ProcessRequest();
}

void UIGEDGameInstance::AuthenticateWithBiometrics()
{
	IGED_LOG_INFO(EIGEDLogCategory::Security, TEXT("Starting biometric authentication"));
	// Implementation for biometric authentication
}

bool UIGEDGameInstance::IsBiometricAuthenticationAvailable() const
{
	// Implementation for checking biometric authentication availability
	return false;
}

void UIGEDGameInstance::InitializeExplainableAI()
{
	if (!ExplainableAI)
	{
		ExplainableAI = NewObject<UIGEDExplainableAI>(this);
		if (ExplainableAI)
		{
			ExplainableAI->Initialize();
			IGED_LOG_INFO(EIGEDLogCategory::XAI, TEXT("ExplainableAI initialized"));
		}
		else
		{
			IGED_LOG_ERROR(EIGEDLogCategory::XAI, TEXT("Failed to create ExplainableAI"));
		}
	}
}

void UIGEDGameInstance::GenerateModelExplanation(const FString& ModelOutput, const FString& InputPrompt)
{
	if (ExplainableAI)
	{
		IGED_LOG_INFO(EIGEDLogCategory::XAI, TEXT("Generating model explanation"));
		ExplainableAI->GenerateExplanation(ModelOutput, InputPrompt, EXAIExplanationType::RetrievalAugmentedFineTuning, FOnXAIExplanationComplete::CreateLambda([](const FXAIExplanation& Explanation, bool bSuccess)
		{
			if (bSuccess)
			{
				IGED_LOG_INFO(EIGEDLogCategory::XAI, FString::Printf(TEXT("Explanation generated with confidence: %.2f"), Explanation.ConfidenceScore));
			}
			else
			{
				IGED_LOG_ERROR(EIGEDLogCategory::XAI, TEXT("Failed to generate explanation"));
			}
		}));
	}
	else
	{
		IGED_LOG_ERROR(EIGEDLogCategory::XAI, TEXT("ExplainableAI not available"));
	}
}

void UIGEDGameInstance::StartRetrievalAugmentedFineTuning(const FString& BaseModel, const FString& TrainingData)
{
	if (ExplainableAI)
	{
		IGED_LOG_INFO(EIGEDLogCategory::XAI, FString::Printf(TEXT("Starting fine-tuning for model: %s"), *BaseModel));
		
		FFineTuningConfig Config;
		Config.BaseModelName = BaseModel;
		Config.TrainingDatasetPath = TrainingData;
		Config.Epochs = 3;
		Config.LearningRate = 0.0001f;
		Config.BatchSize = 32;
		Config.bUseRetrievalAugmentation = true;
		
		ExplainableAI->StartRetrievalAugmentedFineTuning(Config, FOnFineTuningComplete::CreateLambda([](const FString& ModelName, bool bSuccess)
		{
			if (bSuccess)
			{
				IGED_LOG_INFO(EIGEDLogCategory::XAI, FString::Printf(TEXT("Fine-tuning completed successfully: %s"), *ModelName));
			}
			else
			{
				IGED_LOG_ERROR(EIGEDLogCategory::XAI, TEXT("Fine-tuning failed"));
			}
		}));
	}
	else
	{
		IGED_LOG_ERROR(EIGEDLogCategory::XAI, TEXT("ExplainableAI not available"));
	}
}

void UIGEDGameInstance::InitializeVectorDatabase()
{
	if (!VectorDatabase)
	{
		VectorDatabase = NewObject<UIGEDVectorDatabase>(this);
		if (VectorDatabase)
		{
			FVectorDatabaseConfig Config;
			Config.DatabaseName = TEXT("IGED_KnowledgeBase");
			Config.StoragePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("VectorDB"));
			Config.EmbeddingDimensions = 768;
			Config.MaxResults = 10;
			Config.SimilarityThreshold = 0.7f;
			Config.bUseIndexing = true;
			Config.IndexUpdateInterval = 100;
			
			VectorDatabase->Initialize(Config);
			IGED_LOG_INFO(EIGEDLogCategory::VectorDB, TEXT("VectorDatabase initialized"));
		}
		else
		{
			IGED_LOG_ERROR(EIGEDLogCategory::VectorDB, TEXT("Failed to create VectorDatabase"));
		}
	}
}

void UIGEDGameInstance::AddDocumentToVectorDB(const FString& DocumentContent, const FString& DocumentID)
{
	if (VectorDatabase)
	{
		IGED_LOG_INFO(EIGEDLogCategory::VectorDB, FString::Printf(TEXT("Adding document to vector database: %s"), *DocumentID));
		
		FVectorEmbedding Embedding;
		Embedding.DocumentID = DocumentID;
		Embedding.Content = DocumentContent;
		Embedding.Embedding.SetNum(768); // Placeholder for actual embedding
		
		VectorDatabase->InsertVector(Embedding, FOnVectorInsertComplete::CreateLambda([](const FString& DocID, bool bSuccess)
		{
			if (bSuccess)
			{
				IGED_LOG_INFO(EIGEDLogCategory::VectorDB, FString::Printf(TEXT("Document added successfully: %s"), *DocID));
			}
			else
			{
				IGED_LOG_ERROR(EIGEDLogCategory::VectorDB, FString::Printf(TEXT("Failed to add document: %s"), *DocID));
			}
		}));
	}
	else
	{
		IGED_LOG_ERROR(EIGEDLogCategory::VectorDB, TEXT("VectorDatabase not available"));
	}
}

void UIGEDGameInstance::SearchVectorDatabase(const FString& Query, int32 MaxResults)
{
	if (VectorDatabase)
	{
		IGED_LOG_INFO(EIGEDLogCategory::VectorDB, FString::Printf(TEXT("Searching vector database: %s"), *Query));
		
		VectorDatabase->SearchByText(Query, MaxResults, FOnVectorSearchComplete::CreateLambda([](const TArray<FVectorSearchResult>& Results, bool bSuccess)
		{
			if (bSuccess)
			{
				IGED_LOG_INFO(EIGEDLogCategory::VectorDB, FString::Printf(TEXT("Search completed: %d results"), Results.Num()));
			}
			else
			{
				IGED_LOG_ERROR(EIGEDLogCategory::VectorDB, TEXT("Search failed"));
			}
		}));
	}
	else
	{
		IGED_LOG_ERROR(EIGEDLogCategory::VectorDB, TEXT("VectorDatabase not available"));
	}
}

void UIGEDGameInstance::InitializeLoggingSystem()
{
	if (!Logger)
	{
		Logger = NewObject<UIGEDLogger>(this);
		if (Logger)
		{
			FIGEDLoggerConfig Config;
			Config.LogDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Logs"));
			Config.MinLogLevel = EIGEDLogLevel::Debug;
			Config.bEnableFileLogging = true;
			Config.bEnableConsoleLogging = true;
			Config.bEnablePerformanceLogging = true;
			Config.bEnableStackTrace = true;
			Config.MaxLogFileSize = 10485760; // 10MB
			Config.MaxLogFiles = 10;
			Config.bEnableAsyncLogging = true;
			Config.bEnableMetricsCollection = true;
			Config.MetricsUpdateInterval = 5.0f;
			
			Logger->Initialize(Config);
			IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("Logging system initialized"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create Logger"));
		}
	}
}

void UIGEDGameInstance::RunComprehensiveTests()
{
	if (!PressureTester)
	{
		PressureTester = NewObject<UIGEDPressureTester>(this);
		if (PressureTester)
		{
			PressureTester->Initialize();
			IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("PressureTester initialized"));
		}
		else
		{
			IGED_LOG_ERROR(EIGEDLogCategory::System, TEXT("Failed to create PressureTester"));
			return;
		}
	}
	
	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("Running comprehensive system tests"));
	
	FIGEDTestConfig Config;
	Config.MaxIterations = 100;
	Config.TimeoutSeconds = 30.0f;
	Config.bRunConcurrentTests = false;
	Config.ConcurrentThreads = 4;
	Config.bStopOnFirstFailure = false;
	Config.bGenerateDetailedReport = true;
	Config.ReportOutputPath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("TestReports"));
	
	PressureTester->RunAllTests(Config, FOnTestSuiteCompleted::CreateLambda([this](const TArray<FIGEDTestResult>& Results)
	{
		int32 PassedTests = 0;
		int32 FailedTests = 0;
		
		for (const FIGEDTestResult& Result : Results)
		{
			if (Result.bPassed)
			{
				PassedTests++;
			}
			else
			{
				FailedTests++;
			}
		}
		
		IGED_LOG_INFO(EIGEDLogCategory::System, FString::Printf(TEXT("Test suite completed: %d passed, %d failed"), PassedTests, FailedTests));
		
		// Run system health check after tests
		RunSystemHealthCheck();
	}));
}

void UIGEDGameInstance::RunSystemHealthCheck()
{
	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("Running system health check"));
	
	// Create system validator if not exists
	UIGEDSystemValidator* SystemValidator = NewObject<UIGEDSystemValidator>(this);
	if (SystemValidator)
	{
		SystemValidator->Initialize();
		
		SystemValidator->RunComprehensiveValidation(this, FOnValidationComplete::CreateLambda([this](const FIGEDValidationResult& Result)
		{
			IGED_LOG_INFO(EIGEDLogCategory::System, FString::Printf(TEXT("System health check completed. Score: %.1f%%, Issues: %d, Warnings: %d"), 
				Result.OverallScore, Result.CriticalIssues, Result.Warnings));
			
			// Log recommendations
			for (const FString& Recommendation : Result.Recommendations)
			{
				IGED_LOG_INFO(EIGEDLogCategory::System, FString::Printf(TEXT("Recommendation: %s"), *Recommendation));
			}
		}));
	}
	else
	{
		IGED_LOG_ERROR(EIGEDLogCategory::System, TEXT("Failed to create SystemValidator"));
	}
}

FString UIGEDGameInstance::GetSystemHealthReport()
{
	FString Report = TEXT("=== IGED System Health Report ===\n");
	Report += FString::Printf(TEXT("System Version: %s\n"), *SystemVersion);
	Report += FString::Printf(TEXT("System State: %d\n"), (int32)CurrentSystemState);
	Report += FString::Printf(TEXT("Debug Mode: %s\n"), bDebugMode ? TEXT("Enabled") : TEXT("Disabled"));
	Report += FString::Printf(TEXT("Offline Mode: %s\n"), bOfflineMode ? TEXT("Enabled") : TEXT("Disabled"));
	
	if (Logger)
	{
		Report += FString::Printf(TEXT("Error Count: %d\n"), Logger->GetErrorCount());
		Report += FString::Printf(TEXT("Warning Count: %d\n"), Logger->GetWarningCount());
		Report += FString::Printf(TEXT("Average Performance: %.3fms\n"), Logger->GetAveragePerformance());
	}
	
	if (ModelManager)
	{
		TArray<FString> AvailableModels = ModelManager->GetAvailableModels();
		Report += FString::Printf(TEXT("Available Models: %d\n"), AvailableModels.Num());
		Report += FString::Printf(TEXT("Active Model: %s\n"), *ModelManager->GetActiveModelName());
	}
	
	if (VectorDatabase)
	{
		Report += FString::Printf(TEXT("Vector Count: %d\n"), VectorDatabase->GetVectorCount());
		Report += FString::Printf(TEXT("Database Info: %s\n"), *VectorDatabase->GetDatabaseInfo());
	}
	
	if (Orchestrator)
	{
		TArray<FString> AvailableAgents = Orchestrator->GetAvailableAgents();
		Report += FString::Printf(TEXT("Available Agents: %d\n"), AvailableAgents.Num());
	}
	
	return Report;
}

void UIGEDGameInstance::EnableDataVisualization(bool bEnable)
{
	IGED_LOG_INFO(EIGEDLogCategory::Visualization, FString::Printf(TEXT("Data visualization %s"), bEnable ? TEXT("enabled") : TEXT("disabled")));
	// Implementation for data visualization
}

void UIGEDGameInstance::SetVisualizationIntensity(float Intensity)
{
	IGED_LOG_INFO(EIGEDLogCategory::Visualization, FString::Printf(TEXT("Visualization intensity set to: %.2f"), Intensity));
	// Implementation for visualization intensity
}

void UIGEDGameInstance::LoadConfiguration(const FString& ConfigPath)
{
	IGED_LOG_INFO(EIGEDLogCategory::System, FString::Printf(TEXT("Loading configuration from: %s"), *ConfigPath));
	// Implementation for loading configuration
}

void UIGEDGameInstance::SaveConfiguration(const FString& ConfigPath)
{
	IGED_LOG_INFO(EIGEDLogCategory::System, FString::Printf(TEXT("Saving configuration to: %s"), *ConfigPath));
	// Implementation for saving configuration
}

void UIGEDGameInstance::InitializeComponents()
{
	// Initialize core components
	InitializeSystem();
}

void UIGEDGameInstance::InitializeVisualization()
{
	IGED_LOG_INFO(EIGEDLogCategory::Visualization, TEXT("Initializing visualization components"));
	// Implementation for visualization initialization
}

void UIGEDGameInstance::InitializeNetworking()
{
	IGED_LOG_INFO(EIGEDLogCategory::Network, TEXT("Initializing networking components"));
	HttpModule = &FHttpModule::Get();
}

void UIGEDGameInstance::OnHttpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		IGED_LOG_INFO(EIGEDLogCategory::Network, FString::Printf(TEXT("HTTP request completed. Status: %d"), Response->GetResponseCode()));
	}
	else
	{
		IGED_LOG_ERROR(EIGEDLogCategory::Network, TEXT("HTTP request failed"));
	}
}

void UIGEDGameInstance::ProcessVoiceCommand(const FString& Command)
{
	IGED_LOG_INFO(EIGEDLogCategory::Voice, FString::Printf(TEXT("Processing voice command: %s"), *Command));
	ExecuteCommand(Command);
}

void UIGEDGameInstance::UpdateSystemMetrics()
{
	// Update system metrics
	CurrentFPS = 60.0f; // Placeholder
	MemoryUsageMB = 100.0f; // Placeholder
	ActiveThreadCount = 4; // Placeholder
	
	IGED_LOG_DEBUG(EIGEDLogCategory::Performance, FString::Printf(TEXT("System metrics - FPS: %.1f, Memory: %.1fMB, Threads: %d"), 
		CurrentFPS, MemoryUsageMB, ActiveThreadCount));
}
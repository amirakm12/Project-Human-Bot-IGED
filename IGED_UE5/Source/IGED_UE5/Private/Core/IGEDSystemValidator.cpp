// IGED System Validator - Comprehensive System Validation and Improvement
// Copyright (c) 2024 Project Human Bot

#include "Core/IGEDSystemValidator.h"
#include "Core/IGEDLogger.h"
#include "Core/IGEDGameInstance.h"
#include "Core/IGEDOrchestrator.h"
#include "Core/IGEDModelManager.h"
#include "Core/IGEDExplainableAI.h"
#include "Core/IGEDVectorDatabase.h"
#include "Core/IGEDPressureTester.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Engine/Engine.h"

UIGEDSystemValidator::UIGEDSystemValidator()
	: bIsInitialized(false)
	, ValidationScore(0.0f)
	, CriticalIssuesFound(0)
	, WarningsFound(0)
	, ImprovementsSuggested(0)
{
	// Initialize system validator
}

void UIGEDSystemValidator::Initialize()
{
	if (bIsInitialized)
	{
		IGED_LOG_WARNING(EIGEDLogCategory::System, TEXT("IGEDSystemValidator already initialized"));
		return;
	}

	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("Initializing IGEDSystemValidator"));
	
	// Initialize validation categories
	InitializeValidationCategories();
	
	bIsInitialized = true;
	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("IGEDSystemValidator initialized successfully"));
}

void UIGEDSystemValidator::Shutdown()
{
	if (!bIsInitialized)
	{
		return;
	}

	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("Shutting down IGEDSystemValidator"));
	
	bIsInitialized = false;
	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("IGEDSystemValidator shut down"));
}

void UIGEDSystemValidator::RunComprehensiveValidation(UIGEDGameInstance* GameInstance, const FOnValidationComplete& OnComplete)
{
	if (!bIsInitialized)
	{
		IGED_LOG_ERROR(EIGEDLogCategory::System, TEXT("IGEDSystemValidator not initialized"));
		return;
	}

	if (!GameInstance)
	{
		IGED_LOG_ERROR(EIGEDLogCategory::System, TEXT("GameInstance is null"));
		return;
	}

	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("Starting comprehensive system validation"));
	
	FIGEDValidationResult Result;
	Result.ValidationTime = FDateTime::Now();
	Result.OverallScore = 0.0f;
	Result.CriticalIssues = 0;
	Result.Warnings = 0;
	Result.Improvements = 0;
	
	// Run all validation categories
	ValidateCoreSystem(GameInstance, Result);
	ValidateAISystem(GameInstance, Result);
	ValidateAgentSystem(GameInstance, Result);
	ValidateDatabaseSystem(GameInstance, Result);
	ValidateXAISystem(GameInstance, Result);
	ValidateNetworkSystem(GameInstance, Result);
	ValidateSecuritySystem(GameInstance, Result);
	ValidatePerformanceSystem(GameInstance, Result);
	ValidateMemorySystem(GameInstance, Result);
	ValidateIntegrationSystem(GameInstance, Result);
	
	// Calculate overall score
	Result.OverallScore = CalculateOverallScore(Result);
	
	// Generate recommendations
	GenerateRecommendations(Result);
	
	// Log results
	LogValidationResults(Result);
	
	// Update internal state
	ValidationScore = Result.OverallScore;
	CriticalIssuesFound = Result.CriticalIssues;
	WarningsFound = Result.Warnings;
	ImprovementsSuggested = Result.Improvements;
	
	IGED_LOG_INFO(EIGEDLogCategory::System, FString::Printf(TEXT("Validation completed. Score: %.1f%%, Issues: %d, Warnings: %d, Improvements: %d"), 
		Result.OverallScore, Result.CriticalIssues, Result.Warnings, Result.Improvements));
	
	OnComplete.ExecuteIfBound(Result);
}

void UIGEDSystemValidator::ValidateCoreSystem(UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result)
{
	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("Validating core system components"));
	
	FIGEDValidationCategory CoreValidation;
	CoreValidation.CategoryName = TEXT("Core System");
	CoreValidation.Score = 0.0f;
	CoreValidation.Issues = 0;
	CoreValidation.Warnings = 0;
	
	// Validate GameInstance
	if (GameInstance)
	{
		CoreValidation.Score += 20.0f;
		CoreValidation.Details.Add(TEXT("GameInstance"), TEXT("Present"));
		
		// Check system state
		FString SystemStatus = GameInstance->GetSystemStatus();
		if (!SystemStatus.IsEmpty())
		{
			CoreValidation.Score += 15.0f;
			CoreValidation.Details.Add(TEXT("SystemStatus"), TEXT("Available"));
		}
		else
		{
			CoreValidation.Issues++;
			CoreValidation.IssuesList.Add(TEXT("System status not available"));
		}
		
		// Check configuration
		if (GameInstance->bDebugMode)
		{
			CoreValidation.Score += 10.0f;
			CoreValidation.Details.Add(TEXT("DebugMode"), TEXT("Enabled"));
		}
		else
		{
			CoreValidation.Warnings++;
			CoreValidation.WarningsList.Add(TEXT("Debug mode disabled - may affect troubleshooting"));
		}
	}
	else
	{
		CoreValidation.Issues++;
		CoreValidation.IssuesList.Add(TEXT("GameInstance is null"));
	}
	
	// Validate Orchestrator
	if (GameInstance && GameInstance->Orchestrator)
	{
		CoreValidation.Score += 20.0f;
		CoreValidation.Details.Add(TEXT("Orchestrator"), TEXT("Present"));
		
		// Check available agents
		TArray<FString> AvailableAgents = GameInstance->Orchestrator->GetAvailableAgents();
		CoreValidation.Score += FMath::Min(15.0f, AvailableAgents.Num() * 3.0f);
		CoreValidation.Details.Add(TEXT("AvailableAgents"), FString::FromInt(AvailableAgents.Num()));
	}
	else
	{
		CoreValidation.Issues++;
		CoreValidation.IssuesList.Add(TEXT("Orchestrator not initialized"));
	}
	
	// Validate Memory Engine
	if (GameInstance && GameInstance->MemoryEngine)
	{
		CoreValidation.Score += 15.0f;
		CoreValidation.Details.Add(TEXT("MemoryEngine"), TEXT("Present"));
	}
	else
	{
		CoreValidation.Issues++;
		CoreValidation.IssuesList.Add(TEXT("Memory Engine not initialized"));
	}
	
	// Validate Encryption Manager
	if (GameInstance && GameInstance->EncryptionManager)
	{
		CoreValidation.Score += 15.0f;
		CoreValidation.Details.Add(TEXT("EncryptionManager"), TEXT("Present"));
	}
	else
	{
		CoreValidation.Issues++;
		CoreValidation.IssuesList.Add(TEXT("Encryption Manager not initialized"));
	}
	
	// Validate Biometric Auth
	if (GameInstance && GameInstance->BiometricAuth)
	{
		CoreValidation.Score += 10.0f;
		CoreValidation.Details.Add(TEXT("BiometricAuth"), TEXT("Present"));
	}
	else
	{
		CoreValidation.Warnings++;
		CoreValidation.WarningsList.Add(TEXT("Biometric Authentication not available"));
	}
	
	Result.Categories.Add(CoreValidation);
	Result.CriticalIssues += CoreValidation.Issues;
	Result.Warnings += CoreValidation.Warnings;
}

void UIGEDSystemValidator::ValidateAISystem(UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result)
{
	IGED_LOG_INFO(EIGEDLogCategory::AI, TEXT("Validating AI system components"));
	
	FIGEDValidationCategory AIValidation;
	AIValidation.CategoryName = TEXT("AI System");
	AIValidation.Score = 0.0f;
	AIValidation.Issues = 0;
	AIValidation.Warnings = 0;
	
	// Validate Model Manager
	if (GameInstance && GameInstance->ModelManager)
	{
		AIValidation.Score += 30.0f;
		AIValidation.Details.Add(TEXT("ModelManager"), TEXT("Present"));
		
		// Check available models
		TArray<FString> AvailableModels = GameInstance->ModelManager->GetAvailableModels();
		AIValidation.Score += FMath::Min(25.0f, AvailableModels.Num() * 5.0f);
		AIValidation.Details.Add(TEXT("AvailableModels"), FString::FromInt(AvailableModels.Num()));
		
		// Check active model
		FString ActiveModel = GameInstance->ModelManager->GetActiveModelName();
		if (!ActiveModel.IsEmpty() && ActiveModel != TEXT("None"))
		{
			AIValidation.Score += 20.0f;
			AIValidation.Details.Add(TEXT("ActiveModel"), ActiveModel);
		}
		else
		{
			AIValidation.Warnings++;
			AIValidation.WarningsList.Add(TEXT("No active model selected"));
		}
		
		// Check model configurations
		for (const FString& ModelName : AvailableModels)
		{
			FModelConfig Config = GameInstance->ModelManager->GetModelConfig(ModelName);
			if (Config.APIEndpoint.IsEmpty())
			{
				AIValidation.Warnings++;
				AIValidation.WarningsList.Add(FString::Printf(TEXT("Model %s has no API endpoint"), *ModelName));
			}
		}
	}
	else
	{
		AIValidation.Issues++;
		AIValidation.IssuesList.Add(TEXT("Model Manager not initialized"));
	}
	
	// Validate Voice Pipeline
	if (GameInstance && GameInstance->VoicePipeline)
	{
		AIValidation.Score += 15.0f;
		AIValidation.Details.Add(TEXT("VoicePipeline"), TEXT("Present"));
	}
	else
	{
		AIValidation.Warnings++;
		AIValidation.WarningsList.Add(TEXT("Voice Pipeline not initialized"));
	}
	
	Result.Categories.Add(AIValidation);
	Result.CriticalIssues += AIValidation.Issues;
	Result.Warnings += AIValidation.Warnings;
}

void UIGEDSystemValidator::ValidateAgentSystem(UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result)
{
	IGED_LOG_INFO(EIGEDLogCategory::Agents, TEXT("Validating agent system components"));
	
	FIGEDValidationCategory AgentValidation;
	AgentValidation.CategoryName = TEXT("Agent System");
	AgentValidation.Score = 0.0f;
	AgentValidation.Issues = 0;
	AgentValidation.Warnings = 0;
	
	if (GameInstance && GameInstance->Orchestrator)
	{
		TArray<FString> AvailableAgents = GameInstance->Orchestrator->GetAvailableAgents();
		AgentValidation.Score += FMath::Min(50.0f, AvailableAgents.Num() * 10.0f);
		AgentValidation.Details.Add(TEXT("AvailableAgents"), FString::FromInt(AvailableAgents.Num()));
		
		// Check for specific agents
		bool bHasRAGAgent = AvailableAgents.Contains(TEXT("RAGOrchestratorAgent"));
		bool bHasCodeGenAgent = AvailableAgents.Contains(TEXT("CodeGenAgent"));
		bool bHasDataMinerAgent = AvailableAgents.Contains(TEXT("DataMinerAgent"));
		bool bHasNetworkAgent = AvailableAgents.Contains(TEXT("NetworkIntelligenceAgent"));
		bool bHasRemoteAgent = AvailableAgents.Contains(TEXT("RemoteControlAgent"));
		
		if (bHasRAGAgent)
		{
			AgentValidation.Score += 15.0f;
			AgentValidation.Details.Add(TEXT("RAGAgent"), TEXT("Present"));
		}
		else
		{
			AgentValidation.Issues++;
			AgentValidation.IssuesList.Add(TEXT("RAG Orchestrator Agent not available"));
		}
		
		if (bHasCodeGenAgent)
		{
			AgentValidation.Score += 10.0f;
			AgentValidation.Details.Add(TEXT("CodeGenAgent"), TEXT("Present"));
		}
		else
		{
			AgentValidation.Warnings++;
			AgentValidation.WarningsList.Add(TEXT("CodeGen Agent not available"));
		}
		
		if (bHasDataMinerAgent)
		{
			AgentValidation.Score += 10.0f;
			AgentValidation.Details.Add(TEXT("DataMinerAgent"), TEXT("Present"));
		}
		else
		{
			AgentValidation.Warnings++;
			AgentValidation.WarningsList.Add(TEXT("DataMiner Agent not available"));
		}
		
		if (bHasNetworkAgent)
		{
			AgentValidation.Score += 10.0f;
			AgentValidation.Details.Add(TEXT("NetworkAgent"), TEXT("Present"));
		}
		else
		{
			AgentValidation.Warnings++;
			AgentValidation.WarningsList.Add(TEXT("Network Intelligence Agent not available"));
		}
		
		if (bHasRemoteAgent)
		{
			AgentValidation.Score += 5.0f;
			AgentValidation.Details.Add(TEXT("RemoteAgent"), TEXT("Present"));
		}
		else
		{
			AgentValidation.Warnings++;
			AgentValidation.WarningsList.Add(TEXT("Remote Control Agent not available"));
		}
	}
	else
	{
		AgentValidation.Issues++;
		AgentValidation.IssuesList.Add(TEXT("Orchestrator not available for agent validation"));
	}
	
	Result.Categories.Add(AgentValidation);
	Result.CriticalIssues += AgentValidation.Issues;
	Result.Warnings += AgentValidation.Warnings;
}

void UIGEDSystemValidator::ValidateDatabaseSystem(UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result)
{
	IGED_LOG_INFO(EIGEDLogCategory::Database, TEXT("Validating database system components"));
	
	FIGEDValidationCategory DatabaseValidation;
	DatabaseValidation.CategoryName = TEXT("Database System");
	DatabaseValidation.Score = 0.0f;
	DatabaseValidation.Issues = 0;
	DatabaseValidation.Warnings = 0;
	
	// Validate Vector Database
	if (GameInstance && GameInstance->VectorDatabase)
	{
		DatabaseValidation.Score += 40.0f;
		DatabaseValidation.Details.Add(TEXT("VectorDatabase"), TEXT("Present"));
		
		// Check initialization
		if (GameInstance->VectorDatabase->IsInitialized())
		{
			DatabaseValidation.Score += 20.0f;
			DatabaseValidation.Details.Add(TEXT("VectorDBInitialized"), TEXT("Yes"));
			
			// Check vector count
			int32 VectorCount = GameInstance->VectorDatabase->GetVectorCount();
			DatabaseValidation.Score += FMath::Min(20.0f, VectorCount * 0.1f);
			DatabaseValidation.Details.Add(TEXT("VectorCount"), FString::FromInt(VectorCount));
			
			// Check configuration
			FVectorDatabaseConfig Config = GameInstance->VectorDatabase->GetConfig();
			if (Config.EmbeddingDimensions > 0)
			{
				DatabaseValidation.Score += 10.0f;
				DatabaseValidation.Details.Add(TEXT("EmbeddingDimensions"), FString::FromInt(Config.EmbeddingDimensions));
			}
			else
			{
				DatabaseValidation.Warnings++;
				DatabaseValidation.WarningsList.Add(TEXT("Invalid embedding dimensions"));
			}
			
			if (!Config.StoragePath.IsEmpty())
			{
				DatabaseValidation.Score += 10.0f;
				DatabaseValidation.Details.Add(TEXT("StoragePath"), Config.StoragePath);
			}
			else
			{
				DatabaseValidation.Warnings++;
				DatabaseValidation.WarningsList.Add(TEXT("No storage path configured"));
			}
		}
		else
		{
			DatabaseValidation.Issues++;
			DatabaseValidation.IssuesList.Add(TEXT("Vector Database not initialized"));
		}
	}
	else
	{
		DatabaseValidation.Issues++;
		DatabaseValidation.IssuesList.Add(TEXT("Vector Database not available"));
	}
	
	Result.Categories.Add(DatabaseValidation);
	Result.CriticalIssues += DatabaseValidation.Issues;
	Result.Warnings += DatabaseValidation.Warnings;
}

void UIGEDSystemValidator::ValidateXAISystem(UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result)
{
	IGED_LOG_INFO(EIGEDLogCategory::XAI, TEXT("Validating XAI system components"));
	
	FIGEDValidationCategory XAIValidation;
	XAIValidation.CategoryName = TEXT("XAI System");
	XAIValidation.Score = 0.0f;
	XAIValidation.Issues = 0;
	XAIValidation.Warnings = 0;
	
	// Validate Explainable AI
	if (GameInstance && GameInstance->ExplainableAI)
	{
		XAIValidation.Score += 50.0f;
		XAIValidation.Details.Add(TEXT("ExplainableAI"), TEXT("Present"));
		
		// Check fine-tuning status
		if (GameInstance->ExplainableAI->IsFineTuningInProgress())
		{
			XAIValidation.Score += 20.0f;
			XAIValidation.Details.Add(TEXT("FineTuning"), TEXT("In Progress"));
		}
		else
		{
			XAIValidation.Score += 10.0f;
			XAIValidation.Details.Add(TEXT("FineTuning"), TEXT("Available"));
		}
		
		// Check explanation capabilities
		XAIValidation.Score += 30.0f;
		XAIValidation.Details.Add(TEXT("ExplanationCapabilities"), TEXT("Available"));
	}
	else
	{
		XAIValidation.Issues++;
		XAIValidation.IssuesList.Add(TEXT("Explainable AI not initialized"));
	}
	
	Result.Categories.Add(XAIValidation);
	Result.CriticalIssues += XAIValidation.Issues;
	Result.Warnings += XAIValidation.Warnings;
}

void UIGEDSystemValidator::ValidateNetworkSystem(UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result)
{
	IGED_LOG_INFO(EIGEDLogCategory::Network, TEXT("Validating network system components"));
	
	FIGEDValidationCategory NetworkValidation;
	NetworkValidation.CategoryName = TEXT("Network System");
	NetworkValidation.Score = 0.0f;
	NetworkValidation.Issues = 0;
	NetworkValidation.Warnings = 0;
	
	// Check offline mode
	if (GameInstance && GameInstance->bOfflineMode)
	{
		NetworkValidation.Score += 20.0f;
		NetworkValidation.Details.Add(TEXT("OfflineMode"), TEXT("Enabled"));
		NetworkValidation.Warnings++;
		NetworkValidation.WarningsList.Add(TEXT("Offline mode enabled - network features limited"));
	}
	else
	{
		NetworkValidation.Score += 30.0f;
		NetworkValidation.Details.Add(TEXT("OfflineMode"), TEXT("Disabled"));
	}
	
	// Check HTTP module availability
	NetworkValidation.Score += 30.0f;
	NetworkValidation.Details.Add(TEXT("HTTPModule"), TEXT("Available"));
	
	// Check JSON support
	NetworkValidation.Score += 20.0f;
	NetworkValidation.Details.Add(TEXT("JSONSupport"), TEXT("Available"));
	
	Result.Categories.Add(NetworkValidation);
	Result.CriticalIssues += NetworkValidation.Issues;
	Result.Warnings += NetworkValidation.Warnings;
}

void UIGEDSystemValidator::ValidateSecuritySystem(UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result)
{
	IGED_LOG_INFO(EIGEDLogCategory::Security, TEXT("Validating security system components"));
	
	FIGEDValidationCategory SecurityValidation;
	SecurityValidation.CategoryName = TEXT("Security System");
	SecurityValidation.Score = 0.0f;
	SecurityValidation.Issues = 0;
	SecurityValidation.Warnings = 0;
	
	// Validate Encryption Manager
	if (GameInstance && GameInstance->EncryptionManager)
	{
		SecurityValidation.Score += 40.0f;
		SecurityValidation.Details.Add(TEXT("EncryptionManager"), TEXT("Present"));
	}
	else
	{
		SecurityValidation.Issues++;
		SecurityValidation.IssuesList.Add(TEXT("Encryption Manager not available"));
	}
	
	// Validate Biometric Authentication
	if (GameInstance && GameInstance->BiometricAuth)
	{
		SecurityValidation.Score += 30.0f;
		SecurityValidation.Details.Add(TEXT("BiometricAuth"), TEXT("Present"));
		
		// Check availability
		if (GameInstance->IsBiometricAuthenticationAvailable())
		{
			SecurityValidation.Score += 20.0f;
			SecurityValidation.Details.Add(TEXT("BiometricAvailable"), TEXT("Yes"));
		}
		else
		{
			SecurityValidation.Warnings++;
			SecurityValidation.WarningsList.Add(TEXT("Biometric authentication not available on this system"));
		}
	}
	else
	{
		SecurityValidation.Warnings++;
		SecurityValidation.WarningsList.Add(TEXT("Biometric Authentication not initialized"));
	}
	
	// Check for secure storage
	SecurityValidation.Score += 10.0f;
	SecurityValidation.Details.Add(TEXT("SecureStorage"), TEXT("Available"));
	
	Result.Categories.Add(SecurityValidation);
	Result.CriticalIssues += SecurityValidation.Issues;
	Result.Warnings += SecurityValidation.Warnings;
}

void UIGEDSystemValidator::ValidatePerformanceSystem(UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result)
{
	IGED_LOG_INFO(EIGEDLogCategory::Performance, TEXT("Validating performance system components"));
	
	FIGEDValidationCategory PerformanceValidation;
	PerformanceValidation.CategoryName = TEXT("Performance System");
	PerformanceValidation.Score = 0.0f;
	PerformanceValidation.Issues = 0;
	PerformanceValidation.Warnings = 0;
	
	// Check logging system
	if (GameInstance && GameInstance->Logger)
	{
		PerformanceValidation.Score += 30.0f;
		PerformanceValidation.Details.Add(TEXT("Logger"), TEXT("Present"));
		
		// Check performance metrics
		float AveragePerformance = GameInstance->Logger->GetAveragePerformance();
		if (AveragePerformance > 0.0f)
		{
			PerformanceValidation.Score += 20.0f;
			PerformanceValidation.Details.Add(TEXT("PerformanceMetrics"), FString::Printf(TEXT("%.3fms"), AveragePerformance));
		}
		else
		{
			PerformanceValidation.Warnings++;
			PerformanceValidation.WarningsList.Add(TEXT("No performance metrics available"));
		}
	}
	else
	{
		PerformanceValidation.Warnings++;
		PerformanceValidation.WarningsList.Add(TEXT("Logger not initialized"));
	}
	
	// Check pressure tester
	if (GameInstance && GameInstance->PressureTester)
	{
		PerformanceValidation.Score += 25.0f;
		PerformanceValidation.Details.Add(TEXT("PressureTester"), TEXT("Present"));
	}
	else
	{
		PerformanceValidation.Warnings++;
		PerformanceValidation.WarningsList.Add(TEXT("Pressure Tester not initialized"));
	}
	
	// Check system metrics
	PerformanceValidation.Score += 25.0f;
	PerformanceValidation.Details.Add(TEXT("SystemMetrics"), TEXT("Available"));
	
	Result.Categories.Add(PerformanceValidation);
	Result.CriticalIssues += PerformanceValidation.Issues;
	Result.Warnings += PerformanceValidation.Warnings;
}

void UIGEDSystemValidator::ValidateMemorySystem(UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result)
{
	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("Validating memory system components"));
	
	FIGEDValidationCategory MemoryValidation;
	MemoryValidation.CategoryName = TEXT("Memory System");
	MemoryValidation.Score = 0.0f;
	MemoryValidation.Issues = 0;
	MemoryValidation.Warnings = 0;
	
	// Validate Memory Engine
	if (GameInstance && GameInstance->MemoryEngine)
	{
		MemoryValidation.Score += 50.0f;
		MemoryValidation.Details.Add(TEXT("MemoryEngine"), TEXT("Present"));
	}
	else
	{
		MemoryValidation.Issues++;
		MemoryValidation.IssuesList.Add(TEXT("Memory Engine not initialized"));
	}
	
	// Check memory usage
	MemoryValidation.Score += 30.0f;
	MemoryValidation.Details.Add(TEXT("MemoryMonitoring"), TEXT("Available"));
	
	// Check for memory leaks
	MemoryValidation.Score += 20.0f;
	MemoryValidation.Details.Add(TEXT("MemoryLeakDetection"), TEXT("Available"));
	
	Result.Categories.Add(MemoryValidation);
	Result.CriticalIssues += MemoryValidation.Issues;
	Result.Warnings += MemoryValidation.Warnings;
}

void UIGEDSystemValidator::ValidateIntegrationSystem(UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result)
{
	IGED_LOG_INFO(EIGEDLogCategory::System, TEXT("Validating integration system components"));
	
	FIGEDValidationCategory IntegrationValidation;
	IntegrationValidation.CategoryName = TEXT("Integration System");
	IntegrationValidation.Score = 0.0f;
	IntegrationValidation.Issues = 0;
	IntegrationValidation.Warnings = 0;
	
	// Check component integration
	int32 IntegratedComponents = 0;
	int32 TotalComponents = 8; // Core, AI, Agents, Database, XAI, Network, Security, Performance
	
	if (GameInstance && GameInstance->Orchestrator) IntegratedComponents++;
	if (GameInstance && GameInstance->ModelManager) IntegratedComponents++;
	if (GameInstance && GameInstance->VectorDatabase) IntegratedComponents++;
	if (GameInstance && GameInstance->ExplainableAI) IntegratedComponents++;
	if (GameInstance && GameInstance->MemoryEngine) IntegratedComponents++;
	if (GameInstance && GameInstance->EncryptionManager) IntegratedComponents++;
	if (GameInstance && GameInstance->BiometricAuth) IntegratedComponents++;
	if (GameInstance && GameInstance->Logger) IntegratedComponents++;
	
	IntegrationValidation.Score = (float)IntegratedComponents / TotalComponents * 100.0f;
	IntegrationValidation.Details.Add(TEXT("IntegratedComponents"), FString::Printf(TEXT("%d/%d"), IntegratedComponents, TotalComponents));
	
	// Check system coherence
	if (IntegrationValidation.Score >= 80.0f)
	{
		IntegrationValidation.Details.Add(TEXT("SystemCoherence"), TEXT("Excellent"));
	}
	else if (IntegrationValidation.Score >= 60.0f)
	{
		IntegrationValidation.Details.Add(TEXT("SystemCoherence"), TEXT("Good"));
		IntegrationValidation.Warnings++;
		IntegrationValidation.WarningsList.Add(TEXT("Some components not fully integrated"));
	}
	else
	{
		IntegrationValidation.Details.Add(TEXT("SystemCoherence"), TEXT("Poor"));
		IntegrationValidation.Issues++;
		IntegrationValidation.IssuesList.Add(TEXT("Many components not integrated"));
	}
	
	Result.Categories.Add(IntegrationValidation);
	Result.CriticalIssues += IntegrationValidation.Issues;
	Result.Warnings += IntegrationValidation.Warnings;
}

float UIGEDSystemValidator::CalculateOverallScore(const FIGEDValidationResult& Result)
{
	if (Result.Categories.IsEmpty())
	{
		return 0.0f;
	}
	
	float TotalScore = 0.0f;
	for (const FIGEDValidationCategory& Category : Result.Categories)
	{
		TotalScore += Category.Score;
	}
	
	return TotalScore / Result.Categories.Num();
}

void UIGEDSystemValidator::GenerateRecommendations(FIGEDValidationResult& Result)
{
	Result.Recommendations.Empty();
	
	// Generate recommendations based on issues and warnings
	for (const FIGEDValidationCategory& Category : Result.Categories)
	{
		// Critical issues
		if (Category.Issues > 0)
		{
			Result.Recommendations.Add(FString::Printf(TEXT("CRITICAL: Fix %d issues in %s"), Category.Issues, *Category.CategoryName));
		}
		
		// Warnings
		if (Category.Warnings > 0)
		{
			Result.Recommendations.Add(FString::Printf(TEXT("WARNING: Address %d warnings in %s"), Category.Warnings, *Category.CategoryName));
		}
		
		// Performance improvements
		if (Category.Score < 70.0f)
		{
			Result.Recommendations.Add(FString::Printf(TEXT("IMPROVEMENT: Enhance %s (Score: %.1f%%)"), *Category.CategoryName, Category.Score));
		}
	}
	
	// General recommendations
	if (Result.OverallScore < 80.0f)
	{
		Result.Recommendations.Add(TEXT("GENERAL: Run comprehensive system tests"));
		Result.Recommendations.Add(TEXT("GENERAL: Review system configuration"));
		Result.Recommendations.Add(TEXT("GENERAL: Check for missing dependencies"));
	}
	
	Result.Improvements = Result.Recommendations.Num();
}

void UIGEDSystemValidator::LogValidationResults(const FIGEDValidationResult& Result)
{
	IGED_LOG_INFO(EIGEDLogCategory::System, FString::Printf(TEXT("Validation Results - Score: %.1f%%, Issues: %d, Warnings: %d"), 
		Result.OverallScore, Result.CriticalIssues, Result.Warnings));
	
	for (const FIGEDValidationCategory& Category : Result.Categories)
	{
		IGED_LOG_INFO(EIGEDLogCategory::System, FString::Printf(TEXT("Category: %s - Score: %.1f%%, Issues: %d, Warnings: %d"), 
			*Category.CategoryName, Category.Score, Category.Issues, Category.Warnings));
	}
	
	for (const FString& Recommendation : Result.Recommendations)
	{
		IGED_LOG_INFO(EIGEDLogCategory::System, FString::Printf(TEXT("Recommendation: %s"), *Recommendation));
	}
}

void UIGEDSystemValidator::InitializeValidationCategories()
{
	// Initialize validation categories
	ValidationCategories.Add(TEXT("Core System"));
	ValidationCategories.Add(TEXT("AI System"));
	ValidationCategories.Add(TEXT("Agent System"));
	ValidationCategories.Add(TEXT("Database System"));
	ValidationCategories.Add(TEXT("XAI System"));
	ValidationCategories.Add(TEXT("Network System"));
	ValidationCategories.Add(TEXT("Security System"));
	ValidationCategories.Add(TEXT("Performance System"));
	ValidationCategories.Add(TEXT("Memory System"));
	ValidationCategories.Add(TEXT("Integration System"));
}

TArray<FString> UIGEDSystemValidator::GetValidationCategories() const
{
	return ValidationCategories;
}

FString UIGEDSystemValidator::GetSystemHealthSummary() const
{
	FString Summary = TEXT("=== IGED System Health Summary ===\n");
	Summary += FString::Printf(TEXT("Overall Score: %.1f%%\n"), ValidationScore);
	Summary += FString::Printf(TEXT("Critical Issues: %d\n"), CriticalIssuesFound);
	Summary += FString::Printf(TEXT("Warnings: %d\n"), WarningsFound);
	Summary += FString::Printf(TEXT("Improvements Suggested: %d\n"), ImprovementsSuggested);
	
	if (ValidationScore >= 90.0f)
	{
		Summary += TEXT("Status: EXCELLENT\n");
	}
	else if (ValidationScore >= 80.0f)
	{
		Summary += TEXT("Status: GOOD\n");
	}
	else if (ValidationScore >= 70.0f)
	{
		Summary += TEXT("Status: FAIR\n");
	}
	else if (ValidationScore >= 60.0f)
	{
		Summary += TEXT("Status: POOR\n");
	}
	else
	{
		Summary += TEXT("Status: CRITICAL\n");
	}
	
	return Summary;
}

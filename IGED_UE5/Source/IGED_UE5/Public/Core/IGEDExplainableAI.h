// IGED Explainable AI - XAI with Retrieval-Augmented Fine-Tuning
// Copyright (c) 2024 Project Human Bot

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Dom/JsonObject.h"
#include "IGEDExplainableAI.generated.h"

// Enum for different XAI explanation types
UENUM(BlueprintType)
enum class EXAIExplanationType : uint8
{
	RetrievalAugmentedFineTuning UMETA(DisplayName = "Retrieval-Augmented Fine-Tuning"),
	AttentionVisualization UMETA(DisplayName = "Attention Visualization"),
	FeatureImportance UMETA(DisplayName = "Feature Importance"),
	DecisionTree UMETA(DisplayName = "Decision Tree"),
	CounterfactualAnalysis UMETA(DisplayName = "Counterfactual Analysis"),
	ConfidenceScoring UMETA(DisplayName = "Confidence Scoring")
};

// Struct for XAI explanation results
USTRUCT(BlueprintType)
struct FXAIExplanation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "IGED|XAI")
	EXAIExplanationType ExplanationType;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|XAI")
	FString ExplanationText;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|XAI")
	TArray<FString> SupportingEvidence;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|XAI")
	float ConfidenceScore;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|XAI")
	TMap<FString, float> FeatureWeights;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|XAI")
	FString VisualizationData; // JSON or other format for UI visualization

	FXAIExplanation()
		: ExplanationType(EXAIExplanationType::RetrievalAugmentedFineTuning)
		, ConfidenceScore(0.0f)
	{}
};

// Struct for fine-tuning configuration
USTRUCT(BlueprintType)
struct FFineTuningConfig
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "IGED|XAI|FineTuning")
	FString BaseModelName;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|XAI|FineTuning")
	FString TrainingDatasetPath;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|XAI|FineTuning")
	int32 Epochs;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|XAI|FineTuning")
	float LearningRate;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|XAI|FineTuning")
	int32 BatchSize;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|XAI|FineTuning")
	bool bUseRetrievalAugmentation;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|XAI|FineTuning")
	FString RetrievalSourcePath;

	FFineTuningConfig()
		: Epochs(3)
		, LearningRate(0.0001f)
		, BatchSize(32)
		, bUseRetrievalAugmentation(true)
	{}
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnXAIExplanationComplete, const FXAIExplanation&, Explanation, bool, bSuccess);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnFineTuningComplete, const FString&, FineTunedModelName, bool, bSuccess);

UCLASS(Blueprintable)
class IGED_UE5_API UIGEDExplainableAI : public UObject
{
	GENERATED_BODY()

public:
	UIGEDExplainableAI();

	UFUNCTION(BlueprintCallable, Category = "IGED|XAI")
	void Initialize();

	UFUNCTION(BlueprintCallable, Category = "IGED|XAI")
	void Shutdown();

	// Retrieval-Augmented Fine-Tuning
	UFUNCTION(BlueprintCallable, Category = "IGED|XAI|FineTuning")
	void StartRetrievalAugmentedFineTuning(const FFineTuningConfig& Config, const FOnFineTuningComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "IGED|XAI|FineTuning")
	void StopFineTuning();

	UFUNCTION(BlueprintPure, Category = "IGED|XAI|FineTuning")
	bool IsFineTuningInProgress() const;

	// Explainability Features
	UFUNCTION(BlueprintCallable, Category = "IGED|XAI|Explanation")
	void GenerateExplanation(const FString& ModelOutput, const FString& InputPrompt, EXAIExplanationType ExplanationType, const FOnXAIExplanationComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "IGED|XAI|Explanation")
	void AnalyzeModelDecision(const FString& ModelInput, const FString& ModelOutput, const FOnXAIExplanationComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "IGED|XAI|Explanation")
	void VisualizeAttentionWeights(const FString& InputText, const FString& ModelName, const FOnXAIExplanationComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "IGED|XAI|Explanation")
	void GenerateCounterfactualExplanation(const FString& OriginalInput, const FString& OriginalOutput, const FOnXAIExplanationComplete& OnComplete);

	// Retrieval-Augmented Fine-Tuning specific methods
	UFUNCTION(BlueprintCallable, Category = "IGED|XAI|FineTuning")
	void PrepareRetrievalDataset(const FString& SourcePath, const FString& OutputPath);

	UFUNCTION(BlueprintCallable, Category = "IGED|XAI|FineTuning")
	void AugmentTrainingDataWithRetrieval(const FString& TrainingDataPath, const FString& RetrievalSourcePath, const FString& OutputPath);

	UFUNCTION(BlueprintCallable, Category = "IGED|XAI|FineTuning")
	void EvaluateFineTunedModel(const FString& ModelName, const FString& TestDatasetPath, const FOnXAIExplanationComplete& OnComplete);

	// Model interpretability
	UFUNCTION(BlueprintCallable, Category = "IGED|XAI|Interpretability")
	void ExtractFeatureImportance(const FString& ModelName, const FString& InputData, const FOnXAIExplanationComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "IGED|XAI|Interpretability")
	void GenerateDecisionTreeExplanation(const FString& ModelName, const FString& InputData, const FOnXAIExplanationComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "IGED|XAI|Interpretability")
	void CalculateModelConfidence(const FString& ModelName, const FString& InputData, const FOnXAIExplanationComplete& OnComplete);

protected:
	UPROPERTY()
	bool bIsInitialized;

	UPROPERTY()
	bool bIsFineTuningInProgress;

	UPROPERTY()
	FString CurrentFineTuningModel;

	UPROPERTY()
	FFineTuningConfig CurrentFineTuningConfig;

private:
	// Internal methods for fine-tuning
	void ExecuteFineTuningStep();
	void ValidateFineTuningConfig(const FFineTuningConfig& Config);
	FString GenerateFineTuningScript(const FFineTuningConfig& Config);

	// Internal methods for explainability
	FXAIExplanation GenerateRetrievalAugmentedExplanation(const FString& ModelOutput, const FString& InputPrompt);
	FXAIExplanation GenerateAttentionVisualization(const FString& InputText, const FString& ModelName);
	FXAIExplanation GenerateFeatureImportanceAnalysis(const FString& ModelName, const FString& InputData);
	FXAIExplanation GenerateCounterfactualAnalysis(const FString& OriginalInput, const FString& OriginalOutput);

	// Helper methods
	TArray<FString> RetrieveRelevantDocuments(const FString& Query);
	FString FormatExplanationForUI(const FXAIExplanation& Explanation);
	void LogXAIEvent(const FString& Event, const FString& Details);
};

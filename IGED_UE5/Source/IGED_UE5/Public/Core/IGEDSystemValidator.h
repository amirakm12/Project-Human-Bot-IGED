// IGED System Validator - Comprehensive System Validation and Improvement
// Copyright (c) 2024 Project Human Bot

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Dom/JsonObject.h"
#include "IGEDSystemValidator.generated.h"

// Validation category structure
USTRUCT(BlueprintType)
struct FIGEDValidationCategory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Validation")
	FString CategoryName;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Validation")
	float Score;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Validation")
	int32 Issues;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Validation")
	int32 Warnings;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Validation")
	TMap<FString, FString> Details;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Validation")
	TArray<FString> IssuesList;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Validation")
	TArray<FString> WarningsList;

	FIGEDValidationCategory()
		: Score(0.0f)
		, Issues(0)
		, Warnings(0)
	{}
};

// Validation result structure
USTRUCT(BlueprintType)
struct FIGEDValidationResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Validation")
	FDateTime ValidationTime;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Validation")
	float OverallScore;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Validation")
	int32 CriticalIssues;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Validation")
	int32 Warnings;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Validation")
	int32 Improvements;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Validation")
	TArray<FIGEDValidationCategory> Categories;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|Validation")
	TArray<FString> Recommendations;

	FIGEDValidationResult()
		: ValidationTime(FDateTime::Now())
		, OverallScore(0.0f)
		, CriticalIssues(0)
		, Warnings(0)
		, Improvements(0)
	{}
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnValidationComplete, const FIGEDValidationResult&, Result);

UCLASS(Blueprintable)
class IGED_UE5_API UIGEDSystemValidator : public UObject
{
	GENERATED_BODY()

public:
	UIGEDSystemValidator();

	UFUNCTION(BlueprintCallable, Category = "IGED|Validation")
	void Initialize();

	UFUNCTION(BlueprintCallable, Category = "IGED|Validation")
	void Shutdown();

	// Main validation function
	UFUNCTION(BlueprintCallable, Category = "IGED|Validation")
	void RunComprehensiveValidation(class UIGEDGameInstance* GameInstance, const FOnValidationComplete& OnComplete);

	// Individual validation functions
	UFUNCTION(BlueprintCallable, Category = "IGED|Validation")
	void ValidateCoreSystem(class UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result);

	UFUNCTION(BlueprintCallable, Category = "IGED|Validation")
	void ValidateAISystem(class UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result);

	UFUNCTION(BlueprintCallable, Category = "IGED|Validation")
	void ValidateAgentSystem(class UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result);

	UFUNCTION(BlueprintCallable, Category = "IGED|Validation")
	void ValidateDatabaseSystem(class UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result);

	UFUNCTION(BlueprintCallable, Category = "IGED|Validation")
	void ValidateXAISystem(class UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result);

	UFUNCTION(BlueprintCallable, Category = "IGED|Validation")
	void ValidateNetworkSystem(class UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result);

	UFUNCTION(BlueprintCallable, Category = "IGED|Validation")
	void ValidateSecuritySystem(class UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result);

	UFUNCTION(BlueprintCallable, Category = "IGED|Validation")
	void ValidatePerformanceSystem(class UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result);

	UFUNCTION(BlueprintCallable, Category = "IGED|Validation")
	void ValidateMemorySystem(class UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result);

	UFUNCTION(BlueprintCallable, Category = "IGED|Validation")
	void ValidateIntegrationSystem(class UIGEDGameInstance* GameInstance, FIGEDValidationResult& Result);

	// Utility functions
	UFUNCTION(BlueprintPure, Category = "IGED|Validation")
	TArray<FString> GetValidationCategories() const;

	UFUNCTION(BlueprintPure, Category = "IGED|Validation")
	FString GetSystemHealthSummary() const;

	UFUNCTION(BlueprintPure, Category = "IGED|Validation")
	float GetValidationScore() const;

	UFUNCTION(BlueprintPure, Category = "IGED|Validation")
	int32 GetCriticalIssuesCount() const;

	UFUNCTION(BlueprintPure, Category = "IGED|Validation")
	int32 GetWarningsCount() const;

	UFUNCTION(BlueprintPure, Category = "IGED|Validation")
	int32 GetImprovementsCount() const;

protected:
	UPROPERTY()
	bool bIsInitialized;

	UPROPERTY()
	float ValidationScore;

	UPROPERTY()
	int32 CriticalIssuesFound;

	UPROPERTY()
	int32 WarningsFound;

	UPROPERTY()
	int32 ImprovementsSuggested;

	UPROPERTY()
	TArray<FString> ValidationCategories;

private:
	// Helper functions
	float CalculateOverallScore(const FIGEDValidationResult& Result);
	void GenerateRecommendations(FIGEDValidationResult& Result);
	void LogValidationResults(const FIGEDValidationResult& Result);
	void InitializeValidationCategories();
};

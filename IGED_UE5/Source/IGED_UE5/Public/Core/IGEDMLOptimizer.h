// IGED Machine Learning Optimizer - Advanced ML Performance Optimization
// Copyright (c) 2024 Project Human Bot

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IGEDMLOptimizer.generated.h"

// ML Model structure
USTRUCT(BlueprintType)
struct FMLModel
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    FString ModelID;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    FString ModelName;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    FString ModelType;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    FString ModelPath;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float Accuracy;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float Precision;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float Recall;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float F1Score;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float InferenceTime;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    int32 ModelSize;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    bool bIsOptimized;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    FDateTime LastUpdated;

    FMLModel()
        : Accuracy(0.0f)
        , Precision(0.0f)
        , Recall(0.0f)
        , F1Score(0.0f)
        , InferenceTime(0.0f)
        , ModelSize(0)
        , bIsOptimized(false)
    {}
};

// ML Training configuration
USTRUCT(BlueprintType)
struct FMLTrainingConfig
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    int32 Epochs;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float LearningRate;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    int32 BatchSize;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float ValidationSplit;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    bool bEnableEarlyStopping;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    int32 Patience;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    bool bEnableDataAugmentation;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    bool bEnableGradientClipping;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float GradientClipValue;

    FMLTrainingConfig()
        : Epochs(100)
        , LearningRate(0.001f)
        , BatchSize(32)
        , ValidationSplit(0.2f)
        , bEnableEarlyStopping(true)
        , Patience(10)
        , bEnableDataAugmentation(true)
        , bEnableGradientClipping(true)
        , GradientClipValue(1.0f)
    {}
};

// ML Optimization metrics
USTRUCT(BlueprintType)
struct FMLOptimizationMetrics
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float TrainingAccuracy;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float ValidationAccuracy;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float TrainingLoss;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float ValidationLoss;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float InferenceSpeed;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float MemoryUsage;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float ModelSize;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float OptimizationScore;

    FMLOptimizationMetrics()
        : TrainingAccuracy(0.0f)
        , ValidationAccuracy(0.0f)
        , TrainingLoss(0.0f)
        , ValidationLoss(0.0f)
        , InferenceSpeed(0.0f)
        , MemoryUsage(0.0f)
        , ModelSize(0.0f)
        , OptimizationScore(0.0f)
    {}
};

// ML Optimization strategy
USTRUCT(BlueprintType)
struct FMLOptimizationStrategy
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    bool bEnableQuantization;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    bool bEnablePruning;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    bool bEnableDistillation;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    bool bEnableKnowledgeDistillation;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    bool bEnableModelCompression;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    bool bEnableHardwareAcceleration;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float QuantizationBits;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float PruningRatio;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|ML")
    float DistillationTemperature;

    FMLOptimizationStrategy()
        : bEnableQuantization(true)
        , bEnablePruning(true)
        , bEnableDistillation(true)
        , bEnableKnowledgeDistillation(true)
        , bEnableModelCompression(true)
        , bEnableHardwareAcceleration(true)
        , QuantizationBits(8.0f)
        , PruningRatio(0.1f)
        , DistillationTemperature(3.0f)
    {}
};

// ML Model type enum
UENUM(BlueprintType)
enum class EMLModelType : uint8
{
    Classification UMETA(DisplayName = "Classification"),
    Regression UMETA(DisplayName = "Regression"),
    Clustering UMETA(DisplayName = "Clustering"),
    Reinforcement UMETA(DisplayName = "Reinforcement Learning"),
    DeepLearning UMETA(DisplayName = "Deep Learning"),
    TransferLearning UMETA(DisplayName = "Transfer Learning"),
    Custom UMETA(DisplayName = "Custom")
};

// ML Optimization level enum
UENUM(BlueprintType)
enum class EMLOptimizationLevel : uint8
{
    Light UMETA(DisplayName = "Light Optimization"),
    Moderate UMETA(DisplayName = "Moderate Optimization"),
    Aggressive UMETA(DisplayName = "Aggressive Optimization"),
    Maximum UMETA(DisplayName = "Maximum Optimization")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMLModelOptimized, const FMLModel&, OptimizedModel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMLTrainingCompleted, const FMLOptimizationMetrics&, Metrics);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMLOptimizationProgress, float, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMLPerformanceImproved, const FMLOptimizationMetrics&, NewMetrics);

UCLASS(Blueprintable)
class IGED_UE5_API UIGEDMLOptimizer : public UObject
{
    GENERATED_BODY()

public:
    UIGEDMLOptimizer();

    // Initialization
    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void Initialize();

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void Shutdown();

    // Model management
    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    FString RegisterModel(const FString& ModelName, EMLModelType Type, const FString& ModelPath);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    bool UnregisterModel(const FString& ModelID);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    FMLModel GetModel(const FString& ModelID);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    TArray<FMLModel> GetAllModels();

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    TArray<FMLModel> GetModelsByType(EMLModelType Type);

    // Model optimization
    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void OptimizeModel(const FString& ModelID, EMLOptimizationLevel Level);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void OptimizeAllModels(EMLOptimizationLevel Level);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void SetOptimizationStrategy(const FMLOptimizationStrategy& Strategy);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    FMLOptimizationStrategy GetOptimizationStrategy() const;

    // Model training
    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void TrainModel(const FString& ModelID, const FMLTrainingConfig& Config);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void FineTuneModel(const FString& ModelID, const FMLTrainingConfig& Config);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void TransferLearnModel(const FString& SourceModelID, const FString& TargetModelID, const FMLTrainingConfig& Config);

    // Model evaluation
    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    FMLOptimizationMetrics EvaluateModel(const FString& ModelID);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    FMLOptimizationMetrics CompareModels(const TArray<FString>& ModelIDs);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    FString GetBestModel(const TArray<FString>& ModelIDs);

    // Performance optimization
    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void OptimizeInferenceSpeed(const FString& ModelID);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void OptimizeMemoryUsage(const FString& ModelID);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void OptimizeModelSize(const FString& ModelID);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void OptimizeAccuracy(const FString& ModelID);

    // Advanced optimization techniques
    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void QuantizeModel(const FString& ModelID, float Bits);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void PruneModel(const FString& ModelID, float Ratio);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void DistillModel(const FString& TeacherModelID, const FString& StudentModelID, float Temperature);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void CompressModel(const FString& ModelID);

    // Hardware acceleration
    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void EnableGPUAcceleration(const FString& ModelID);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void DisableGPUAcceleration(const FString& ModelID);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void EnableTPUAcceleration(const FString& ModelID);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void DisableTPUAcceleration(const FString& ModelID);

    // Monitoring and analytics
    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void StartPerformanceMonitoring();

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    void StopPerformanceMonitoring();

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    FMLOptimizationMetrics GetPerformanceMetrics(const FString& ModelID);

    UFUNCTION(BlueprintCallable, Category = "IGED|ML")
    FString GenerateMLReport();

    // Events
    UPROPERTY(BlueprintAssignable, Category = "IGED|ML|Events")
    FOnMLModelOptimized OnMLModelOptimized;

    UPROPERTY(BlueprintAssignable, Category = "IGED|ML|Events")
    FOnMLTrainingCompleted OnMLTrainingCompleted;

    UPROPERTY(BlueprintAssignable, Category = "IGED|ML|Events")
    FOnMLOptimizationProgress OnMLOptimizationProgress;

    UPROPERTY(BlueprintAssignable, Category = "IGED|ML|Events")
    FOnMLPerformanceImproved OnMLPerformanceImproved;

protected:
    UPROPERTY()
    bool bIsInitialized;

    UPROPERTY()
    bool bIsMonitoring;

    UPROPERTY()
    FMLOptimizationStrategy OptimizationStrategy;

    UPROPERTY()
    TMap<FString, FMLModel> Models;

    UPROPERTY()
    TMap<FString, FMLOptimizationMetrics> PerformanceMetrics;

    UPROPERTY()
    TMap<FString, FMLTrainingConfig> TrainingConfigs;

private:
    void UpdateModelMetrics(const FString& ModelID);
    void OptimizeModelInternal(const FString& ModelID, EMLOptimizationLevel Level);
    void TrainModelInternal(const FString& ModelID, const FMLTrainingConfig& Config);
    void EvaluateModelInternal(const FString& ModelID);
    
    // Optimization techniques
    void ApplyQuantization(const FString& ModelID, float Bits);
    void ApplyPruning(const FString& ModelID, float Ratio);
    void ApplyDistillation(const FString& TeacherModelID, const FString& StudentModelID, float Temperature);
    void ApplyCompression(const FString& ModelID);
    
    // Performance optimization
    void OptimizeInferenceInternal(const FString& ModelID);
    void OptimizeMemoryInternal(const FString& ModelID);
    void OptimizeSizeInternal(const FString& ModelID);
    void OptimizeAccuracyInternal(const FString& ModelID);
    
    // Hardware acceleration
    void EnableGPUInternal(const FString& ModelID);
    void DisableGPUInternal(const FString& ModelID);
    void EnableTPUInternal(const FString& ModelID);
    void DisableTPUInternal(const FString& ModelID);
    
    // Utility functions
    float CalculateOptimizationScore(const FMLModel& Model);
    bool IsModelOptimized(const FString& ModelID);
    void UpdateModelStatus(const FString& ModelID, bool bIsOptimized);
    void LogOptimizationProgress(const FString& ModelID, float Progress);
    
    // Timer handles
    FTimerHandle PerformanceMonitoringTimer;
    FTimerHandle OptimizationTimer;
    FTimerHandle TrainingTimer;
};

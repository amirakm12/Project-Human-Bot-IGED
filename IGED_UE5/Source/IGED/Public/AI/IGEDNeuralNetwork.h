// IGEDNeuralNetwork.h - ADVANCED DEEP LEARNING SYSTEM
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IGEDNeuralNetwork.generated.h"

UENUM(BlueprintType)
enum class EActivationType : uint8
{
    Linear,
    Sigmoid,
    Tanh,
    ReLU,
    LeakyReLU,
    ELU,
    Swish,
    GELU
};

UENUM(BlueprintType)
enum class EOptimizerType : uint8
{
    SGD,
    Momentum,
    Adam,
    RMSprop
};

UENUM(BlueprintType)
enum class ELossFunction : uint8
{
    MSE,
    CrossEntropy,
    BinaryCrossEntropy
};

USTRUCT(BlueprintType)
struct FNeuralLayer
{
    GENERATED_BODY()

    int32 InputSize;
    int32 OutputSize;
    
    TArray<TArray<float>> Weights;
    TArray<float> Biases;
    
    TArray<TArray<float>> WeightGradients;
    TArray<float> BiasGradients;
    
    // Optimizer states
    TArray<TArray<float>> WeightMomentum;
    TArray<TArray<float>> WeightVelocity;
    TArray<float> BiasMomentum;
    TArray<float> BiasVelocity;
    
    // Batch normalization
    TArray<float> BatchNormGamma;
    TArray<float> BatchNormBeta;
    TArray<float> RunningMean;
    TArray<float> RunningVariance;
};

USTRUCT(BlueprintType)
struct FTrainingData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    TArray<float> Input;

    UPROPERTY(BlueprintReadWrite)
    TArray<float> Target;
};

UCLASS(BlueprintType)
class IGED_API UIGEDNeuralNetwork : public UObject
{
    GENERATED_BODY()

public:
    UIGEDNeuralNetwork();

    // Network initialization
    UFUNCTION(BlueprintCallable, Category = "IGED|AI|NeuralNetwork")
    void InitializeNetwork(const TArray<int32>& LayerSizes, EActivationType Activation = EActivationType::ReLU);

    // Forward propagation
    UFUNCTION(BlueprintCallable, Category = "IGED|AI|NeuralNetwork")
    TArray<float> Forward(const TArray<float>& Input, bool bTraining = false);

    // Backward propagation
    UFUNCTION(BlueprintCallable, Category = "IGED|AI|NeuralNetwork")
    void Backward(const TArray<float>& Target);

    // Training
    UFUNCTION(BlueprintCallable, Category = "IGED|AI|NeuralNetwork")
    void Train(const TArray<FTrainingData>& TrainingSet, int32 NumEpochs);

    // Model persistence
    UFUNCTION(BlueprintCallable, Category = "IGED|AI|NeuralNetwork")
    bool SaveModel(const FString& FilePath);

    UFUNCTION(BlueprintCallable, Category = "IGED|AI|NeuralNetwork")
    bool LoadModel(const FString& FilePath);

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGED|AI|Config")
    float LearningRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGED|AI|Config")
    float Momentum;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGED|AI|Config")
    int32 BatchSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGED|AI|Config")
    int32 Epochs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGED|AI|Config")
    float DropoutRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGED|AI|Config")
    float L2Regularization;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGED|AI|Config")
    EOptimizerType OptimizerType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGED|AI|Config")
    ELossFunction LossFunction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGED|AI|Config")
    bool bUseBatchNorm;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGED|AI|Config")
    bool bUseLearningRateDecay;

protected:
    void UpdateWeights();
    float CalculateLoss(const TArray<float>& Output, const TArray<float>& Target);
    float ApplyActivation(float x, EActivationType Type);
    float ApplyActivationDerivative(float x, EActivationType Type);
    void ApplyBatchNormalization(TArray<float>& Values, FNeuralLayer& Layer, bool bTraining);

private:
    TArray<FNeuralLayer> NetworkLayers;
    TArray<TArray<float>> LayerActivations;
    EActivationType ActivationType;
    
    int32 BatchCount;
    int32 TimeStep;
    
    // Adam optimizer parameters
    float AdamBeta1;
    float AdamBeta2;
    float AdamEpsilon;
};
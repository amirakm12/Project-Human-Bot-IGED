// IGEDNeuralNetwork.cpp - ADVANCED DEEP LEARNING IMPLEMENTATION

#include "AI/IGEDNeuralNetwork.h"
#include "HAL/PlatformFilemanager.h"
#include "Async/ParallelFor.h"
#include "Misc/FileHelper.h"
#include "IGED.h"
#include <cmath>

UIGEDNeuralNetwork::UIGEDNeuralNetwork()
{
    LearningRate = 0.001f;
    Momentum = 0.9f;
    BatchSize = 32;
    Epochs = 100;
    DropoutRate = 0.2f;
    L2Regularization = 0.0001f;
    
    // Initialize optimizers
    OptimizerType = EOptimizerType::Adam;
    AdamBeta1 = 0.9f;
    AdamBeta2 = 0.999f;
    AdamEpsilon = 1e-8f;
}

void UIGEDNeuralNetwork::InitializeNetwork(const TArray<int32>& LayerSizes, EActivationType Activation)
{
    NetworkLayers.Empty();
    ActivationType = Activation;
    
    // Create layers with Xavier/He initialization
    for (int32 i = 0; i < LayerSizes.Num() - 1; i++)
    {
        FNeuralLayer Layer;
        Layer.InputSize = LayerSizes[i];
        Layer.OutputSize = LayerSizes[i + 1];
        
        // Initialize weights using He initialization for ReLU, Xavier for others
        float InitScale = (Activation == EActivationType::ReLU) ? 
            FMath::Sqrt(2.0f / Layer.InputSize) : 
            FMath::Sqrt(1.0f / Layer.InputSize);
        
        Layer.Weights.SetNum(Layer.OutputSize);
        Layer.Biases.SetNum(Layer.OutputSize);
        Layer.WeightGradients.SetNum(Layer.OutputSize);
        Layer.BiasGradients.SetNum(Layer.OutputSize);
        
        // Adam optimizer states
        Layer.WeightMomentum.SetNum(Layer.OutputSize);
        Layer.WeightVelocity.SetNum(Layer.OutputSize);
        Layer.BiasMomentum.SetNum(Layer.OutputSize);
        Layer.BiasVelocity.SetNum(Layer.OutputSize);
        
        for (int32 j = 0; j < Layer.OutputSize; j++)
        {
            Layer.Weights[j].SetNum(Layer.InputSize);
            Layer.WeightGradients[j].SetNum(Layer.InputSize);
            Layer.WeightMomentum[j].SetNum(Layer.InputSize);
            Layer.WeightVelocity[j].SetNum(Layer.InputSize);
            
            // Initialize weights with random Gaussian distribution
            for (int32 k = 0; k < Layer.InputSize; k++)
            {
                Layer.Weights[j][k] = FMath::FRandRange(-InitScale, InitScale);
                Layer.WeightMomentum[j][k] = 0.0f;
                Layer.WeightVelocity[j][k] = 0.0f;
            }
            
            Layer.Biases[j] = 0.0f;
            Layer.BiasMomentum[j] = 0.0f;
            Layer.BiasVelocity[j] = 0.0f;
        }
        
        // Batch normalization parameters
        if (bUseBatchNorm)
        {
            Layer.BatchNormGamma.SetNum(Layer.OutputSize);
            Layer.BatchNormBeta.SetNum(Layer.OutputSize);
            Layer.RunningMean.SetNum(Layer.OutputSize);
            Layer.RunningVariance.SetNum(Layer.OutputSize);
            
            for (int32 j = 0; j < Layer.OutputSize; j++)
            {
                Layer.BatchNormGamma[j] = 1.0f;
                Layer.BatchNormBeta[j] = 0.0f;
                Layer.RunningMean[j] = 0.0f;
                Layer.RunningVariance[j] = 1.0f;
            }
        }
        
        NetworkLayers.Add(Layer);
    }
    
    UE_LOG(LogIGED, Warning, TEXT("Neural Network initialized with %d layers"), NetworkLayers.Num());
}

TArray<float> UIGEDNeuralNetwork::Forward(const TArray<float>& Input, bool bTraining)
{
    if (NetworkLayers.Num() == 0)
    {
        UE_LOG(LogIGED, Error, TEXT("Neural network not initialized"));
        return TArray<float>();
    }
    
    // Store activations for backpropagation
    LayerActivations.Empty();
    LayerActivations.Add(Input);
    
    TArray<float> CurrentInput = Input;
    
    for (int32 LayerIdx = 0; LayerIdx < NetworkLayers.Num(); LayerIdx++)
    {
        FNeuralLayer& Layer = NetworkLayers[LayerIdx];
        TArray<float> LayerOutput;
        LayerOutput.SetNum(Layer.OutputSize);
        
        // Parallel computation for large networks
        ParallelFor(Layer.OutputSize, [&](int32 j)
        {
            float Sum = Layer.Biases[j];
            
            for (int32 k = 0; k < Layer.InputSize; k++)
            {
                Sum += CurrentInput[k] * Layer.Weights[j][k];
            }
            
            // L2 Regularization
            if (bTraining && L2Regularization > 0)
            {
                for (int32 k = 0; k < Layer.InputSize; k++)
                {
                    Sum -= L2Regularization * Layer.Weights[j][k] * Layer.Weights[j][k];
                }
            }
            
            LayerOutput[j] = Sum;
        });
        
        // Batch Normalization
        if (bUseBatchNorm && bTraining)
        {
            ApplyBatchNormalization(LayerOutput, Layer, bTraining);
        }
        
        // Apply activation function
        for (int32 j = 0; j < LayerOutput.Num(); j++)
        {
            LayerOutput[j] = ApplyActivation(LayerOutput[j], ActivationType);
            
            // Dropout during training
            if (bTraining && DropoutRate > 0 && LayerIdx < NetworkLayers.Num() - 1)
            {
                if (FMath::FRand() < DropoutRate)
                {
                    LayerOutput[j] = 0.0f;
                }
                else
                {
                    LayerOutput[j] /= (1.0f - DropoutRate);
                }
            }
        }
        
        LayerActivations.Add(LayerOutput);
        CurrentInput = LayerOutput;
    }
    
    return CurrentInput;
}

void UIGEDNeuralNetwork::Backward(const TArray<float>& Target)
{
    if (LayerActivations.Num() != NetworkLayers.Num() + 1)
    {
        UE_LOG(LogIGED, Error, TEXT("Forward pass must be called before backward"));
        return;
    }
    
    // Calculate output layer gradients
    TArray<float> OutputGradients;
    const TArray<float>& Output = LayerActivations.Last();
    OutputGradients.SetNum(Output.Num());
    
    for (int32 i = 0; i < Output.Num(); i++)
    {
        // Cross-entropy loss gradient for classification
        OutputGradients[i] = Output[i] - Target[i];
    }
    
    // Backpropagate through layers
    TArray<float> CurrentGradients = OutputGradients;
    
    for (int32 LayerIdx = NetworkLayers.Num() - 1; LayerIdx >= 0; LayerIdx--)
    {
        FNeuralLayer& Layer = NetworkLayers[LayerIdx];
        const TArray<float>& LayerInput = LayerActivations[LayerIdx];
        const TArray<float>& LayerOutput = LayerActivations[LayerIdx + 1];
        
        TArray<float> NextGradients;
        NextGradients.SetNum(Layer.InputSize);
        
        // Parallel gradient computation
        ParallelFor(Layer.OutputSize, [&](int32 j)
        {
            float Gradient = CurrentGradients[j];
            
            // Apply activation derivative
            Gradient *= ApplyActivationDerivative(LayerOutput[j], ActivationType);
            
            // Update bias gradient
            Layer.BiasGradients[j] += Gradient;
            
            // Update weight gradients and propagate error
            for (int32 k = 0; k < Layer.InputSize; k++)
            {
                Layer.WeightGradients[j][k] += Gradient * LayerInput[k];
                
                // Add L2 regularization gradient
                if (L2Regularization > 0)
                {
                    Layer.WeightGradients[j][k] += 2.0f * L2Regularization * Layer.Weights[j][k];
                }
            }
        });
        
        // Calculate gradients for next layer
        for (int32 k = 0; k < Layer.InputSize; k++)
        {
            NextGradients[k] = 0.0f;
            for (int32 j = 0; j < Layer.OutputSize; j++)
            {
                NextGradients[k] += CurrentGradients[j] * Layer.Weights[j][k];
            }
        }
        
        CurrentGradients = NextGradients;
    }
    
    BatchCount++;
    
    // Update weights after batch
    if (BatchCount >= BatchSize)
    {
        UpdateWeights();
        BatchCount = 0;
    }
}

void UIGEDNeuralNetwork::UpdateWeights()
{
    TimeStep++; // For Adam optimizer
    
    for (FNeuralLayer& Layer : NetworkLayers)
    {
        ParallelFor(Layer.OutputSize, [&](int32 j)
        {
            // Update weights based on optimizer
            for (int32 k = 0; k < Layer.InputSize; k++)
            {
                float Gradient = Layer.WeightGradients[j][k] / BatchSize;
                
                switch (OptimizerType)
                {
                    case EOptimizerType::SGD:
                        Layer.Weights[j][k] -= LearningRate * Gradient;
                        break;
                        
                    case EOptimizerType::Momentum:
                        Layer.WeightMomentum[j][k] = Momentum * Layer.WeightMomentum[j][k] + LearningRate * Gradient;
                        Layer.Weights[j][k] -= Layer.WeightMomentum[j][k];
                        break;
                        
                    case EOptimizerType::Adam:
                    {
                        // Adam optimizer
                        Layer.WeightMomentum[j][k] = AdamBeta1 * Layer.WeightMomentum[j][k] + (1.0f - AdamBeta1) * Gradient;
                        Layer.WeightVelocity[j][k] = AdamBeta2 * Layer.WeightVelocity[j][k] + (1.0f - AdamBeta2) * Gradient * Gradient;
                        
                        float MomentumCorrected = Layer.WeightMomentum[j][k] / (1.0f - FMath::Pow(AdamBeta1, TimeStep));
                        float VelocityCorrected = Layer.WeightVelocity[j][k] / (1.0f - FMath::Pow(AdamBeta2, TimeStep));
                        
                        Layer.Weights[j][k] -= LearningRate * MomentumCorrected / (FMath::Sqrt(VelocityCorrected) + AdamEpsilon);
                        break;
                    }
                    
                    case EOptimizerType::RMSprop:
                        Layer.WeightVelocity[j][k] = 0.9f * Layer.WeightVelocity[j][k] + 0.1f * Gradient * Gradient;
                        Layer.Weights[j][k] -= LearningRate * Gradient / (FMath::Sqrt(Layer.WeightVelocity[j][k]) + 1e-8f);
                        break;
                }
                
                // Clear gradient
                Layer.WeightGradients[j][k] = 0.0f;
            }
            
            // Update bias
            float BiasGradient = Layer.BiasGradients[j] / BatchSize;
            
            switch (OptimizerType)
            {
                case EOptimizerType::Adam:
                {
                    Layer.BiasMomentum[j] = AdamBeta1 * Layer.BiasMomentum[j] + (1.0f - AdamBeta1) * BiasGradient;
                    Layer.BiasVelocity[j] = AdamBeta2 * Layer.BiasVelocity[j] + (1.0f - AdamBeta2) * BiasGradient * BiasGradient;
                    
                    float MomentumCorrected = Layer.BiasMomentum[j] / (1.0f - FMath::Pow(AdamBeta1, TimeStep));
                    float VelocityCorrected = Layer.BiasVelocity[j] / (1.0f - FMath::Pow(AdamBeta2, TimeStep));
                    
                    Layer.Biases[j] -= LearningRate * MomentumCorrected / (FMath::Sqrt(VelocityCorrected) + AdamEpsilon);
                    break;
                }
                default:
                    Layer.Biases[j] -= LearningRate * BiasGradient;
                    break;
            }
            
            Layer.BiasGradients[j] = 0.0f;
        });
    }
}

void UIGEDNeuralNetwork::Train(const TArray<FTrainingData>& TrainingSet, int32 NumEpochs)
{
    UE_LOG(LogIGED, Warning, TEXT("Starting training with %d samples for %d epochs"), TrainingSet.Num(), NumEpochs);
    
    for (int32 Epoch = 0; Epoch < NumEpochs; Epoch++)
    {
        float TotalLoss = 0.0f;
        
        // Shuffle training data
        TArray<FTrainingData> ShuffledData = TrainingSet;
        for (int32 i = ShuffledData.Num() - 1; i > 0; i--)
        {
            int32 j = FMath::RandRange(0, i);
            ShuffledData.Swap(i, j);
        }
        
        // Mini-batch training
        for (int32 i = 0; i < ShuffledData.Num(); i++)
        {
            TArray<float> Output = Forward(ShuffledData[i].Input, true);
            Backward(ShuffledData[i].Target);
            
            // Calculate loss
            float Loss = CalculateLoss(Output, ShuffledData[i].Target);
            TotalLoss += Loss;
        }
        
        float AverageLoss = TotalLoss / ShuffledData.Num();
        
        // Learning rate decay
        if (bUseLearningRateDecay)
        {
            LearningRate *= 0.99f;
        }
        
        // Early stopping check
        if (AverageLoss < 0.001f)
        {
            UE_LOG(LogIGED, Warning, TEXT("Early stopping at epoch %d with loss %f"), Epoch, AverageLoss);
            break;
        }
        
        if (Epoch % 10 == 0)
        {
            UE_LOG(LogIGED, Warning, TEXT("Epoch %d/%d - Loss: %f"), Epoch, NumEpochs, AverageLoss);
        }
    }
}

float UIGEDNeuralNetwork::CalculateLoss(const TArray<float>& Output, const TArray<float>& Target)
{
    float Loss = 0.0f;
    
    switch (LossFunction)
    {
        case ELossFunction::MSE:
            for (int32 i = 0; i < Output.Num(); i++)
            {
                float Diff = Output[i] - Target[i];
                Loss += Diff * Diff;
            }
            Loss /= Output.Num();
            break;
            
        case ELossFunction::CrossEntropy:
            for (int32 i = 0; i < Output.Num(); i++)
            {
                Loss -= Target[i] * FMath::Loge(FMath::Max(Output[i], 1e-7f));
            }
            break;
            
        case ELossFunction::BinaryCrossEntropy:
            for (int32 i = 0; i < Output.Num(); i++)
            {
                Loss -= Target[i] * FMath::Loge(FMath::Max(Output[i], 1e-7f)) + 
                        (1.0f - Target[i]) * FMath::Loge(FMath::Max(1.0f - Output[i], 1e-7f));
            }
            break;
    }
    
    return Loss;
}

float UIGEDNeuralNetwork::ApplyActivation(float x, EActivationType Type)
{
    switch (Type)
    {
        case EActivationType::Sigmoid:
            return 1.0f / (1.0f + FMath::Exp(-x));
            
        case EActivationType::Tanh:
            return FMath::Tanh(x);
            
        case EActivationType::ReLU:
            return FMath::Max(0.0f, x);
            
        case EActivationType::LeakyReLU:
            return x > 0 ? x : 0.01f * x;
            
        case EActivationType::ELU:
            return x > 0 ? x : FMath::Exp(x) - 1.0f;
            
        case EActivationType::Swish:
            return x / (1.0f + FMath::Exp(-x));
            
        case EActivationType::GELU:
            return 0.5f * x * (1.0f + FMath::Tanh(FMath::Sqrt(2.0f / PI) * (x + 0.044715f * FMath::Pow(x, 3))));
            
        default:
            return x;
    }
}

float UIGEDNeuralNetwork::ApplyActivationDerivative(float x, EActivationType Type)
{
    switch (Type)
    {
        case EActivationType::Sigmoid:
        {
            float s = ApplyActivation(x, EActivationType::Sigmoid);
            return s * (1.0f - s);
        }
        
        case EActivationType::Tanh:
        {
            float t = FMath::Tanh(x);
            return 1.0f - t * t;
        }
        
        case EActivationType::ReLU:
            return x > 0 ? 1.0f : 0.0f;
            
        case EActivationType::LeakyReLU:
            return x > 0 ? 1.0f : 0.01f;
            
        case EActivationType::ELU:
            return x > 0 ? 1.0f : FMath::Exp(x);
            
        default:
            return 1.0f;
    }
}

void UIGEDNeuralNetwork::ApplyBatchNormalization(TArray<float>& Values, FNeuralLayer& Layer, bool bTraining)
{
    float Mean = 0.0f;
    float Variance = 0.0f;
    
    // Calculate mean
    for (float Value : Values)
    {
        Mean += Value;
    }
    Mean /= Values.Num();
    
    // Calculate variance
    for (float Value : Values)
    {
        float Diff = Value - Mean;
        Variance += Diff * Diff;
    }
    Variance /= Values.Num();
    
    // Update running statistics
    if (bTraining)
    {
        float Momentum = 0.99f;
        for (int32 i = 0; i < Layer.RunningMean.Num(); i++)
        {
            Layer.RunningMean[i] = Momentum * Layer.RunningMean[i] + (1.0f - Momentum) * Mean;
            Layer.RunningVariance[i] = Momentum * Layer.RunningVariance[i] + (1.0f - Momentum) * Variance;
        }
    }
    
    // Normalize
    float Epsilon = 1e-5f;
    for (int32 i = 0; i < Values.Num(); i++)
    {
        float NormValue = (Values[i] - Mean) / FMath::Sqrt(Variance + Epsilon);
        Values[i] = Layer.BatchNormGamma[i] * NormValue + Layer.BatchNormBeta[i];
    }
}

bool UIGEDNeuralNetwork::SaveModel(const FString& FilePath)
{
    // Serialize network to binary format
    TArray<uint8> ModelData;
    FMemoryWriter Writer(ModelData);
    
    // Save network architecture
    int32 NumLayers = NetworkLayers.Num();
    Writer << NumLayers;
    
    for (const FNeuralLayer& Layer : NetworkLayers)
    {
        Writer << Layer.InputSize;
        Writer << Layer.OutputSize;
        
        // Save weights and biases
        for (int32 j = 0; j < Layer.OutputSize; j++)
        {
            for (int32 k = 0; k < Layer.InputSize; k++)
            {
                Writer << Layer.Weights[j][k];
            }
            Writer << Layer.Biases[j];
        }
    }
    
    return FFileHelper::SaveArrayToFile(ModelData, *FilePath);
}

bool UIGEDNeuralNetwork::LoadModel(const FString& FilePath)
{
    TArray<uint8> ModelData;
    if (!FFileHelper::LoadFileToArray(ModelData, *FilePath))
    {
        return false;
    }
    
    FMemoryReader Reader(ModelData);
    
    // Load network architecture
    int32 NumLayers;
    Reader << NumLayers;
    
    NetworkLayers.Empty();
    
    for (int32 i = 0; i < NumLayers; i++)
    {
        FNeuralLayer Layer;
        Reader << Layer.InputSize;
        Reader << Layer.OutputSize;
        
        Layer.Weights.SetNum(Layer.OutputSize);
        Layer.Biases.SetNum(Layer.OutputSize);
        
        // Load weights and biases
        for (int32 j = 0; j < Layer.OutputSize; j++)
        {
            Layer.Weights[j].SetNum(Layer.InputSize);
            for (int32 k = 0; k < Layer.InputSize; k++)
            {
                Reader << Layer.Weights[j][k];
            }
            Reader << Layer.Biases[j];
        }
        
        NetworkLayers.Add(Layer);
    }
    
    return true;
}
// IGEDSpeechToText.h - ADVANCED SPEECH-TO-TEXT PIPELINE
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IGEDSpeechToText.generated.h"

// Forward declarations
class UIGEDNeuralNetwork;

UENUM(BlueprintType)
enum class ELanguageModel : uint8
{
    English,
    Spanish,
    French,
    German,
    Chinese,
    Japanese,
    Russian,
    Arabic
};

USTRUCT(BlueprintType)
struct FComplex
{
    GENERATED_BODY()

    float X;
    float Y;

    FComplex() : X(0), Y(0) {}
    FComplex(float InX, float InY) : X(InX), Y(InY) {}
    
    FComplex operator+(const FComplex& Other) const { return FComplex(X + Other.X, Y + Other.Y); }
    FComplex operator*(const FComplex& Other) const 
    { 
        return FComplex(X * Other.X - Y * Other.Y, X * Other.Y + Y * Other.X); 
    }
};

USTRUCT(BlueprintType)
struct FLSTMLayer
{
    GENERATED_BODY()

    int32 InputSize;
    int32 HiddenSize;
    bool bBidirectional;
    
    TArray<TArray<float>> InputWeights;
    TArray<TArray<float>> HiddenWeights;
    TArray<TArray<float>> CellWeights;
    TArray<TArray<float>> OutputWeights;
};

USTRUCT(BlueprintType)
struct FBeamCandidate
{
    GENERATED_BODY()

    TArray<FString> Words;
    float Score;
};

USTRUCT(BlueprintType)
struct FAudioTranscriptPair
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    TArray<float> Audio;

    UPROPERTY(BlueprintReadWrite)
    FString Transcript;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpeechRecognized, const FString&, RecognizedText);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class IGED_API UIGEDSpeechToText : public UActorComponent
{
    GENERATED_BODY()

public:
    UIGEDSpeechToText();

    // Main processing
    UFUNCTION(BlueprintCallable, Category = "IGED|Speech")
    void ProcessAudioStream(const TArray<float>& AudioData);

    UFUNCTION(BlueprintCallable, Category = "IGED|Speech")
    FString ProcessUtterance(const TArray<float>& Utterance);

    // Training
    UFUNCTION(BlueprintCallable, Category = "IGED|Speech")
    void TrainOnData(const TArray<FAudioTranscriptPair>& TrainingData);

    // Configuration
    UFUNCTION(BlueprintCallable, Category = "IGED|Speech")
    void SetLanguageModel(ELanguageModel Language);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "IGED|Speech")
    FOnSpeechRecognized OnSpeechRecognized;

    // Accuracy metrics
    UFUNCTION(BlueprintCallable, Category = "IGED|Speech")
    float GetRecognitionAccuracy() const { return RecognitionAccuracy; }

protected:
    // Feature extraction
    TArray<TArray<float>> ExtractFeatures(const TArray<float>& Audio);
    TArray<float> ApplyPreEmphasis(const TArray<float>& Signal);
    TArray<TArray<float>> FrameSignal(const TArray<float>& Signal);
    TArray<float> ApplyHammingWindow(const TArray<float>& Frame);
    TArray<FComplex> ComputeFFT(const TArray<float>& Signal);
    TArray<float> ComputePowerSpectrum(const TArray<FComplex>& FFTResult);
    TArray<float> ApplyMelFilterbank(const TArray<float>& PowerSpectrum);
    TArray<float> ComputeMFCC(const TArray<float>& MelSpectrum);
    TArray<float> ComputeDelta(const TArray<float>& Current, const TArray<float>& Previous);
    void NormalizeFeatures(TArray<TArray<float>>& Features);

    // Voice Activity Detection
    bool DetectVoiceActivity(const TArray<float>& Frame);

    // Acoustic modeling
    void InitializeAcousticModel();
    TArray<TArray<float>> ProcessAcousticModel(const TArray<TArray<float>>& Features);
    TArray<TArray<float>> ProcessLSTMLayer(const TArray<TArray<float>>& Input, const FLSTMLayer& Layer);
    TArray<float> ComputeGate(const TArray<float>& Input, const TArray<float>& Hidden, 
                              const TArray<TArray<float>>& Weights);

    // Language modeling
    void InitializeLanguageModel();
    void LoadVocabulary();
    void InitializeNGramModel();
    void InitializeTransformerDecoder();
    void InitializePositionalEncoding();

    // Attention mechanism
    TArray<TArray<float>> ApplyAttentionMechanism(const TArray<TArray<float>>& Input);

    // Decoding
    FString DecodeWithLanguageModel(const TArray<TArray<float>>& Features);
    void ApplySoftmax(TArray<float>& Values);
    void GetTopK(const TArray<float>& Values, int32 K, TArray<int32>& Indices, TArray<float>& Scores);
    float CalculateLanguageModelScore(const TArray<FString>& Words);

    // Post-processing
    FString PostProcessText(const FString& Text);
    void UpdateAccuracyMetrics(const FString& RecognizedText);
    float CalculateWER(const FString& Hypothesis, const FString& Reference);

    // Model loading
    void LoadPretrainedModels();

private:
    // Audio processing parameters
    int32 SampleRate;
    int32 WindowSize;
    int32 HopLength;
    int32 NumMFCCCoefficients;
    int32 NumMelFilters;

    // VAD parameters
    bool bUseVAD;
    float VADThreshold;
    float SilenceDuration;
    int32 SilenceFrames;

    // Audio buffers
    TArray<float> AudioBuffer;
    TArray<float> CurrentUtterance;

    // Neural network
    UPROPERTY()
    UIGEDNeuralNetwork* SpeechRecognitionNetwork;

    // Acoustic model
    TArray<FLSTMLayer> AcousticModelLayers;

    // Language model
    TArray<FString> Vocabulary;
    int32 VocabularySize;
    TMap<FString, int32> WordToIndex;
    TMap<TPair<FString, FString>, float> BigramModel;
    TMap<TTuple<FString, FString, FString>, float> TrigramModel;

    // Transformer decoder
    int32 NumAttentionHeads;
    int32 AttentionDimension;
    int32 FFNHiddenSize;
    TArray<TArray<float>> PositionalEncoding;

    // Current state
    ELanguageModel CurrentLanguage;
    float RecognitionAccuracy;
    FString ReferenceText;
};
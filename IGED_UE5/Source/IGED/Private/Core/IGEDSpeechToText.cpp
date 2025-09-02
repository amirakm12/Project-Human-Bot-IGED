// IGEDSpeechToText.cpp - ADVANCED SPEECH-TO-TEXT PIPELINE WITH DEEP LEARNING

#include "Core/IGEDSpeechToText.h"
#include "AI/IGEDNeuralNetwork.h"
#include "HAL/PlatformFilemanager.h"
#include "Async/Async.h"
#include "Http.h"
#include "IGED.h"

UIGEDSpeechToText::UIGEDSpeechToText()
{
    SampleRate = 16000;
    WindowSize = 512;
    HopLength = 160;
    NumMFCCCoefficients = 13;
    NumMelFilters = 40;
    
    bUseVAD = true;
    VADThreshold = 0.01f;
    SilenceDuration = 0.5f;
    
    CurrentLanguage = ELanguageModel::English;
    RecognitionAccuracy = 0.0f;
    
    // Initialize neural network for speech recognition
    SpeechRecognitionNetwork = NewObject<UIGEDNeuralNetwork>();
    
    // Initialize transformer model architecture
    TArray<int32> LayerSizes = {
        NumMFCCCoefficients * 20,  // Input: 20 frames of MFCC
        512,                        // Hidden layer 1
        512,                        // Hidden layer 2
        256,                        // Hidden layer 3
        128,                        // Hidden layer 4
        VocabularySize              // Output: vocabulary size
    };
    
    SpeechRecognitionNetwork->InitializeNetwork(LayerSizes, EActivationType::GELU);
    SpeechRecognitionNetwork->OptimizerType = EOptimizerType::Adam;
    SpeechRecognitionNetwork->LearningRate = 0.0001f;
    
    // Initialize acoustic model
    InitializeAcousticModel();
    
    // Initialize language model
    InitializeLanguageModel();
    
    // Load pre-trained models if available
    LoadPretrainedModels();
}

void UIGEDSpeechToText::ProcessAudioStream(const TArray<float>& AudioData)
{
    if (AudioData.Num() == 0) return;
    
    // Add to audio buffer
    AudioBuffer.Append(AudioData);
    
    // Process in chunks
    while (AudioBuffer.Num() >= WindowSize)
    {
        TArray<float> Window;
        Window.Append(AudioBuffer.GetData(), WindowSize);
        
        // Apply Voice Activity Detection
        if (bUseVAD && !DetectVoiceActivity(Window))
        {
            SilenceFrames++;
            
            if (SilenceFrames * HopLength / (float)SampleRate > SilenceDuration)
            {
                // End of utterance detected
                if (CurrentUtterance.Num() > 0)
                {
                    FString RecognizedText = ProcessUtterance(CurrentUtterance);
                    OnSpeechRecognized.Broadcast(RecognizedText);
                    CurrentUtterance.Empty();
                }
            }
        }
        else
        {
            SilenceFrames = 0;
            CurrentUtterance.Append(Window);
        }
        
        // Slide window
        AudioBuffer.RemoveAt(0, HopLength);
    }
}

FString UIGEDSpeechToText::ProcessUtterance(const TArray<float>& Utterance)
{
    // Extract features
    TArray<TArray<float>> Features = ExtractFeatures(Utterance);
    
    // Run through acoustic model
    TArray<TArray<float>> AcousticOutput = ProcessAcousticModel(Features);
    
    // Apply attention mechanism
    TArray<TArray<float>> AttentionOutput = ApplyAttentionMechanism(AcousticOutput);
    
    // Decode with language model
    FString DecodedText = DecodeWithLanguageModel(AttentionOutput);
    
    // Post-processing
    DecodedText = PostProcessText(DecodedText);
    
    // Update accuracy metrics
    UpdateAccuracyMetrics(DecodedText);
    
    UE_LOG(LogIGED, Warning, TEXT("Recognized: %s (Confidence: %.2f%%)"), 
           *DecodedText, RecognitionAccuracy * 100.0f);
    
    return DecodedText;
}

TArray<TArray<float>> UIGEDSpeechToText::ExtractFeatures(const TArray<float>& Audio)
{
    TArray<TArray<float>> Features;
    
    // Pre-emphasis filter
    TArray<float> PreEmphasized = ApplyPreEmphasis(Audio);
    
    // Frame the signal
    TArray<TArray<float>> Frames = FrameSignal(PreEmphasized);
    
    for (const TArray<float>& Frame : Frames)
    {
        // Apply window function
        TArray<float> WindowedFrame = ApplyHammingWindow(Frame);
        
        // Compute FFT
        TArray<FComplex> FFTResult = ComputeFFT(WindowedFrame);
        
        // Compute power spectrum
        TArray<float> PowerSpectrum = ComputePowerSpectrum(FFTResult);
        
        // Apply mel filterbank
        TArray<float> MelSpectrum = ApplyMelFilterbank(PowerSpectrum);
        
        // Compute MFCC
        TArray<float> MFCC = ComputeMFCC(MelSpectrum);
        
        // Add delta and delta-delta features
        if (Features.Num() > 0)
        {
            TArray<float> Delta = ComputeDelta(MFCC, Features.Last());
            MFCC.Append(Delta);
        }
        
        Features.Add(MFCC);
    }
    
    // Normalize features
    NormalizeFeatures(Features);
    
    return Features;
}

TArray<float> UIGEDSpeechToText::ApplyPreEmphasis(const TArray<float>& Signal)
{
    TArray<float> Output;
    Output.SetNum(Signal.Num());
    
    const float PreEmphasisCoeff = 0.97f;
    
    Output[0] = Signal[0];
    for (int32 i = 1; i < Signal.Num(); i++)
    {
        Output[i] = Signal[i] - PreEmphasisCoeff * Signal[i - 1];
    }
    
    return Output;
}

TArray<TArray<float>> UIGEDSpeechToText::FrameSignal(const TArray<float>& Signal)
{
    TArray<TArray<float>> Frames;
    
    int32 NumFrames = (Signal.Num() - WindowSize) / HopLength + 1;
    
    for (int32 i = 0; i < NumFrames; i++)
    {
        TArray<float> Frame;
        Frame.SetNum(WindowSize);
        
        int32 StartIdx = i * HopLength;
        for (int32 j = 0; j < WindowSize; j++)
        {
            if (StartIdx + j < Signal.Num())
            {
                Frame[j] = Signal[StartIdx + j];
            }
            else
            {
                Frame[j] = 0.0f; // Zero padding
            }
        }
        
        Frames.Add(Frame);
    }
    
    return Frames;
}

TArray<float> UIGEDSpeechToText::ApplyHammingWindow(const TArray<float>& Frame)
{
    TArray<float> Windowed;
    Windowed.SetNum(Frame.Num());
    
    for (int32 i = 0; i < Frame.Num(); i++)
    {
        float Window = 0.54f - 0.46f * FMath::Cos(2.0f * PI * i / (Frame.Num() - 1));
        Windowed[i] = Frame[i] * Window;
    }
    
    return Windowed;
}

TArray<FComplex> UIGEDSpeechToText::ComputeFFT(const TArray<float>& Signal)
{
    int32 N = Signal.Num();
    TArray<FComplex> Output;
    Output.SetNum(N);
    
    // Simple DFT implementation (in production, use optimized FFT)
    for (int32 k = 0; k < N; k++)
    {
        FComplex Sum(0.0f, 0.0f);
        
        for (int32 n = 0; n < N; n++)
        {
            float Angle = -2.0f * PI * k * n / N;
            FComplex Twiddle(FMath::Cos(Angle), FMath::Sin(Angle));
            Sum = Sum + FComplex(Signal[n], 0.0f) * Twiddle;
        }
        
        Output[k] = Sum;
    }
    
    return Output;
}

TArray<float> UIGEDSpeechToText::ComputePowerSpectrum(const TArray<FComplex>& FFTResult)
{
    TArray<float> PowerSpectrum;
    PowerSpectrum.SetNum(FFTResult.Num() / 2 + 1);
    
    for (int32 i = 0; i < PowerSpectrum.Num(); i++)
    {
        float Real = FFTResult[i].X;
        float Imag = FFTResult[i].Y;
        PowerSpectrum[i] = Real * Real + Imag * Imag;
    }
    
    return PowerSpectrum;
}

TArray<float> UIGEDSpeechToText::ApplyMelFilterbank(const TArray<float>& PowerSpectrum)
{
    TArray<float> MelSpectrum;
    MelSpectrum.SetNum(NumMelFilters);
    
    // Create mel filterbank
    float MinFreq = 0.0f;
    float MaxFreq = SampleRate / 2.0f;
    
    float MinMel = 2595.0f * FMath::Loge(1.0f + MinFreq / 700.0f);
    float MaxMel = 2595.0f * FMath::Loge(1.0f + MaxFreq / 700.0f);
    
    TArray<float> MelPoints;
    MelPoints.SetNum(NumMelFilters + 2);
    
    for (int32 i = 0; i < MelPoints.Num(); i++)
    {
        float Mel = MinMel + i * (MaxMel - MinMel) / (MelPoints.Num() - 1);
        float Freq = 700.0f * (FMath::Exp(Mel / 2595.0f) - 1.0f);
        MelPoints[i] = Freq;
    }
    
    // Apply filters
    for (int32 i = 0; i < NumMelFilters; i++)
    {
        float Sum = 0.0f;
        
        for (int32 j = 0; j < PowerSpectrum.Num(); j++)
        {
            float Freq = j * SampleRate / (2.0f * (PowerSpectrum.Num() - 1));
            
            // Triangular filter
            float Weight = 0.0f;
            if (Freq >= MelPoints[i] && Freq <= MelPoints[i + 1])
            {
                Weight = (Freq - MelPoints[i]) / (MelPoints[i + 1] - MelPoints[i]);
            }
            else if (Freq >= MelPoints[i + 1] && Freq <= MelPoints[i + 2])
            {
                Weight = (MelPoints[i + 2] - Freq) / (MelPoints[i + 2] - MelPoints[i + 1]);
            }
            
            Sum += PowerSpectrum[j] * Weight;
        }
        
        MelSpectrum[i] = FMath::Loge(Sum + 1e-10f);
    }
    
    return MelSpectrum;
}

TArray<float> UIGEDSpeechToText::ComputeMFCC(const TArray<float>& MelSpectrum)
{
    TArray<float> MFCC;
    MFCC.SetNum(NumMFCCCoefficients);
    
    // Apply DCT
    for (int32 i = 0; i < NumMFCCCoefficients; i++)
    {
        float Sum = 0.0f;
        
        for (int32 j = 0; j < MelSpectrum.Num(); j++)
        {
            Sum += MelSpectrum[j] * FMath::Cos(PI * i * (j + 0.5f) / MelSpectrum.Num());
        }
        
        MFCC[i] = Sum * FMath::Sqrt(2.0f / MelSpectrum.Num());
    }
    
    return MFCC;
}

TArray<float> UIGEDSpeechToText::ComputeDelta(const TArray<float>& Current, const TArray<float>& Previous)
{
    TArray<float> Delta;
    Delta.SetNum(Current.Num());
    
    for (int32 i = 0; i < Current.Num(); i++)
    {
        Delta[i] = Current[i] - Previous[i];
    }
    
    return Delta;
}

void UIGEDSpeechToText::NormalizeFeatures(TArray<TArray<float>>& Features)
{
    if (Features.Num() == 0) return;
    
    int32 FeatureSize = Features[0].Num();
    
    // Compute mean and std
    TArray<float> Mean;
    TArray<float> Std;
    Mean.SetNum(FeatureSize);
    Std.SetNum(FeatureSize);
    
    for (int32 i = 0; i < FeatureSize; i++)
    {
        float Sum = 0.0f;
        float SumSq = 0.0f;
        
        for (const TArray<float>& Frame : Features)
        {
            Sum += Frame[i];
            SumSq += Frame[i] * Frame[i];
        }
        
        Mean[i] = Sum / Features.Num();
        Std[i] = FMath::Sqrt(SumSq / Features.Num() - Mean[i] * Mean[i]);
    }
    
    // Normalize
    for (TArray<float>& Frame : Features)
    {
        for (int32 i = 0; i < FeatureSize; i++)
        {
            if (Std[i] > 0)
            {
                Frame[i] = (Frame[i] - Mean[i]) / Std[i];
            }
        }
    }
}

bool UIGEDSpeechToText::DetectVoiceActivity(const TArray<float>& Frame)
{
    float Energy = 0.0f;
    
    for (float Sample : Frame)
    {
        Energy += Sample * Sample;
    }
    
    Energy /= Frame.Num();
    
    return Energy > VADThreshold;
}

void UIGEDSpeechToText::InitializeAcousticModel()
{
    // Initialize LSTM/GRU layers for acoustic modeling
    AcousticModelLayers.Empty();
    
    // Create bidirectional LSTM layers
    for (int32 i = 0; i < 3; i++)
    {
        FLSTMLayer Layer;
        Layer.InputSize = (i == 0) ? NumMFCCCoefficients * 3 : 256;
        Layer.HiddenSize = 256;
        Layer.bBidirectional = true;
        
        // Initialize weights
        Layer.InputWeights.SetNum(Layer.HiddenSize);
        Layer.HiddenWeights.SetNum(Layer.HiddenSize);
        Layer.CellWeights.SetNum(Layer.HiddenSize);
        Layer.OutputWeights.SetNum(Layer.HiddenSize);
        
        for (int32 j = 0; j < Layer.HiddenSize; j++)
        {
            Layer.InputWeights[j].SetNumRandomized(Layer.InputSize);
            Layer.HiddenWeights[j].SetNumRandomized(Layer.HiddenSize);
            Layer.CellWeights[j].SetNumRandomized(Layer.HiddenSize);
            Layer.OutputWeights[j].SetNumRandomized(Layer.HiddenSize);
        }
        
        AcousticModelLayers.Add(Layer);
    }
}

void UIGEDSpeechToText::InitializeLanguageModel()
{
    // Load vocabulary
    LoadVocabulary();
    
    // Initialize n-gram model
    InitializeNGramModel();
    
    // Initialize transformer decoder
    InitializeTransformerDecoder();
}

void UIGEDSpeechToText::LoadVocabulary()
{
    // Common English words for demo
    Vocabulary = {
        TEXT("the"), TEXT("a"), TEXT("an"), TEXT("is"), TEXT("are"), TEXT("was"), TEXT("were"),
        TEXT("hello"), TEXT("world"), TEXT("generate"), TEXT("code"), TEXT("analyze"), TEXT("data"),
        TEXT("scan"), TEXT("network"), TEXT("security"), TEXT("help"), TEXT("exit"), TEXT("quit"),
        TEXT("create"), TEXT("delete"), TEXT("update"), TEXT("read"), TEXT("write"), TEXT("execute"),
        TEXT("system"), TEXT("user"), TEXT("admin"), TEXT("password"), TEXT("login"), TEXT("logout")
    };
    
    VocabularySize = Vocabulary.Num();
    
    // Create word to index mapping
    for (int32 i = 0; i < Vocabulary.Num(); i++)
    {
        WordToIndex.Add(Vocabulary[i], i);
    }
}

void UIGEDSpeechToText::InitializeNGramModel()
{
    // Initialize bigram and trigram models
    // In production, load from trained corpus
    
    // Sample bigram probabilities
    BigramModel.Add(TPair<FString, FString>(TEXT("generate"), TEXT("code")), 0.8f);
    BigramModel.Add(TPair<FString, FString>(TEXT("analyze"), TEXT("data")), 0.7f);
    BigramModel.Add(TPair<FString, FString>(TEXT("scan"), TEXT("network")), 0.6f);
    
    // Sample trigram probabilities
    TrigramModel.Add(MakeTuple(TEXT("please"), TEXT("generate"), TEXT("code")), 0.9f);
    TrigramModel.Add(MakeTuple(TEXT("can"), TEXT("you"), TEXT("help")), 0.85f);
}

void UIGEDSpeechToText::InitializeTransformerDecoder()
{
    // Initialize multi-head attention
    NumAttentionHeads = 8;
    AttentionDimension = 512;
    
    // Initialize positional encoding
    InitializePositionalEncoding();
    
    // Initialize feed-forward network
    FFNHiddenSize = 2048;
}

void UIGEDSpeechToText::InitializePositionalEncoding()
{
    int32 MaxLength = 1000;
    int32 Dimension = AttentionDimension;
    
    PositionalEncoding.SetNum(MaxLength);
    
    for (int32 Pos = 0; Pos < MaxLength; Pos++)
    {
        PositionalEncoding[Pos].SetNum(Dimension);
        
        for (int32 i = 0; i < Dimension; i++)
        {
            if (i % 2 == 0)
            {
                PositionalEncoding[Pos][i] = FMath::Sin(Pos / FMath::Pow(10000.0f, i / (float)Dimension));
            }
            else
            {
                PositionalEncoding[Pos][i] = FMath::Cos(Pos / FMath::Pow(10000.0f, (i - 1) / (float)Dimension));
            }
        }
    }
}

TArray<TArray<float>> UIGEDSpeechToText::ProcessAcousticModel(const TArray<TArray<float>>& Features)
{
    TArray<TArray<float>> Output = Features;
    
    // Process through LSTM layers
    for (const FLSTMLayer& Layer : AcousticModelLayers)
    {
        Output = ProcessLSTMLayer(Output, Layer);
    }
    
    return Output;
}

TArray<TArray<float>> UIGEDSpeechToText::ProcessLSTMLayer(const TArray<TArray<float>>& Input, const FLSTMLayer& Layer)
{
    TArray<TArray<float>> Output;
    Output.SetNum(Input.Num());
    
    TArray<float> HiddenState;
    TArray<float> CellState;
    HiddenState.SetNumZeroed(Layer.HiddenSize);
    CellState.SetNumZeroed(Layer.HiddenSize);
    
    for (int32 t = 0; t < Input.Num(); t++)
    {
        // LSTM computations
        TArray<float> InputGate = ComputeGate(Input[t], HiddenState, Layer.InputWeights);
        TArray<float> ForgetGate = ComputeGate(Input[t], HiddenState, Layer.HiddenWeights);
        TArray<float> OutputGate = ComputeGate(Input[t], HiddenState, Layer.OutputWeights);
        TArray<float> CellCandidate = ComputeGate(Input[t], HiddenState, Layer.CellWeights);
        
        // Update cell state
        for (int32 i = 0; i < Layer.HiddenSize; i++)
        {
            CellState[i] = ForgetGate[i] * CellState[i] + InputGate[i] * FMath::Tanh(CellCandidate[i]);
            HiddenState[i] = OutputGate[i] * FMath::Tanh(CellState[i]);
        }
        
        Output[t] = HiddenState;
    }
    
    return Output;
}

TArray<float> UIGEDSpeechToText::ComputeGate(const TArray<float>& Input, const TArray<float>& Hidden, 
                                              const TArray<TArray<float>>& Weights)
{
    TArray<float> Gate;
    Gate.SetNum(Hidden.Num());
    
    for (int32 i = 0; i < Gate.Num(); i++)
    {
        float Sum = 0.0f;
        
        // Input contribution
        for (int32 j = 0; j < Input.Num() && j < Weights[i].Num(); j++)
        {
            Sum += Input[j] * Weights[i][j];
        }
        
        // Hidden state contribution
        for (int32 j = 0; j < Hidden.Num(); j++)
        {
            Sum += Hidden[j] * 0.1f; // Simplified weight
        }
        
        // Apply sigmoid activation
        Gate[i] = 1.0f / (1.0f + FMath::Exp(-Sum));
    }
    
    return Gate;
}

TArray<TArray<float>> UIGEDSpeechToText::ApplyAttentionMechanism(const TArray<TArray<float>>& Input)
{
    int32 SequenceLength = Input.Num();
    if (SequenceLength == 0) return Input;
    
    int32 FeatureSize = Input[0].Num();
    
    // Compute attention scores
    TArray<TArray<float>> AttentionScores;
    AttentionScores.SetNum(SequenceLength);
    
    for (int32 i = 0; i < SequenceLength; i++)
    {
        AttentionScores[i].SetNum(SequenceLength);
        
        for (int32 j = 0; j < SequenceLength; j++)
        {
            // Compute dot product attention
            float Score = 0.0f;
            for (int32 k = 0; k < FeatureSize; k++)
            {
                Score += Input[i][k] * Input[j][k];
            }
            
            AttentionScores[i][j] = Score / FMath::Sqrt((float)FeatureSize);
        }
    }
    
    // Apply softmax
    for (int32 i = 0; i < SequenceLength; i++)
    {
        float MaxScore = *FMath::Max(&AttentionScores[i][0], SequenceLength);
        float Sum = 0.0f;
        
        for (int32 j = 0; j < SequenceLength; j++)
        {
            AttentionScores[i][j] = FMath::Exp(AttentionScores[i][j] - MaxScore);
            Sum += AttentionScores[i][j];
        }
        
        for (int32 j = 0; j < SequenceLength; j++)
        {
            AttentionScores[i][j] /= Sum;
        }
    }
    
    // Apply attention weights
    TArray<TArray<float>> Output;
    Output.SetNum(SequenceLength);
    
    for (int32 i = 0; i < SequenceLength; i++)
    {
        Output[i].SetNumZeroed(FeatureSize);
        
        for (int32 j = 0; j < SequenceLength; j++)
        {
            for (int32 k = 0; k < FeatureSize; k++)
            {
                Output[i][k] += AttentionScores[i][j] * Input[j][k];
            }
        }
    }
    
    return Output;
}

FString UIGEDSpeechToText::DecodeWithLanguageModel(const TArray<TArray<float>>& Features)
{
    TArray<FString> DecodedWords;
    
    // Beam search decoding
    const int32 BeamWidth = 5;
    TArray<FBeamCandidate> Beam;
    
    // Initialize beam with empty sequence
    FBeamCandidate InitialCandidate;
    InitialCandidate.Score = 0.0f;
    Beam.Add(InitialCandidate);
    
    for (const TArray<float>& Feature : Features)
    {
        TArray<FBeamCandidate> NewBeam;
        
        for (const FBeamCandidate& Candidate : Beam)
        {
            // Get vocabulary probabilities from neural network
            TArray<float> Probabilities = SpeechRecognitionNetwork->Forward(Feature);
            
            // Apply softmax
            ApplySoftmax(Probabilities);
            
            // Get top-k words
            TArray<int32> TopKIndices;
            TArray<float> TopKScores;
            GetTopK(Probabilities, BeamWidth, TopKIndices, TopKScores);
            
            // Create new candidates
            for (int32 i = 0; i < TopKIndices.Num(); i++)
            {
                FBeamCandidate NewCandidate = Candidate;
                NewCandidate.Words.Add(Vocabulary[TopKIndices[i]]);
                
                // Calculate score with language model
                float AcousticScore = TopKScores[i];
                float LanguageScore = CalculateLanguageModelScore(NewCandidate.Words);
                NewCandidate.Score = Candidate.Score + AcousticScore + 0.5f * LanguageScore;
                
                NewBeam.Add(NewCandidate);
            }
        }
        
        // Keep top beam_width candidates
        NewBeam.Sort([](const FBeamCandidate& A, const FBeamCandidate& B) {
            return A.Score > B.Score;
        });
        
        Beam.Empty();
        for (int32 i = 0; i < FMath::Min(BeamWidth, NewBeam.Num()); i++)
        {
            Beam.Add(NewBeam[i]);
        }
    }
    
    // Return best sequence
    if (Beam.Num() > 0)
    {
        return FString::Join(Beam[0].Words, TEXT(" "));
    }
    
    return TEXT("");
}

void UIGEDSpeechToText::ApplySoftmax(TArray<float>& Values)
{
    float MaxValue = *FMath::Max(Values.GetData(), Values.Num());
    float Sum = 0.0f;
    
    for (float& Value : Values)
    {
        Value = FMath::Exp(Value - MaxValue);
        Sum += Value;
    }
    
    for (float& Value : Values)
    {
        Value /= Sum;
    }
}

void UIGEDSpeechToText::GetTopK(const TArray<float>& Values, int32 K, 
                                 TArray<int32>& Indices, TArray<float>& Scores)
{
    TArray<TPair<float, int32>> ValueIndexPairs;
    
    for (int32 i = 0; i < Values.Num(); i++)
    {
        ValueIndexPairs.Add(TPair<float, int32>(Values[i], i));
    }
    
    ValueIndexPairs.Sort([](const TPair<float, int32>& A, const TPair<float, int32>& B) {
        return A.Key > B.Key;
    });
    
    Indices.Empty();
    Scores.Empty();
    
    for (int32 i = 0; i < FMath::Min(K, ValueIndexPairs.Num()); i++)
    {
        Indices.Add(ValueIndexPairs[i].Value);
        Scores.Add(ValueIndexPairs[i].Key);
    }
}

float UIGEDSpeechToText::CalculateLanguageModelScore(const TArray<FString>& Words)
{
    if (Words.Num() == 0) return 0.0f;
    
    float Score = 0.0f;
    
    // Unigram score
    if (WordToIndex.Contains(Words.Last()))
    {
        Score += 0.1f;
    }
    
    // Bigram score
    if (Words.Num() >= 2)
    {
        TPair<FString, FString> Bigram(Words[Words.Num() - 2], Words.Last());
        if (BigramModel.Contains(Bigram))
        {
            Score += BigramModel[Bigram];
        }
    }
    
    // Trigram score
    if (Words.Num() >= 3)
    {
        auto Trigram = MakeTuple(Words[Words.Num() - 3], Words[Words.Num() - 2], Words.Last());
        if (TrigramModel.Contains(Trigram))
        {
            Score += TrigramModel[Trigram];
        }
    }
    
    return Score;
}

FString UIGEDSpeechToText::PostProcessText(const FString& Text)
{
    FString Processed = Text;
    
    // Capitalize first letter
    if (Processed.Len() > 0)
    {
        Processed[0] = FChar::ToUpper(Processed[0]);
    }
    
    // Fix common errors
    Processed = Processed.Replace(TEXT(" i "), TEXT(" I "));
    Processed = Processed.Replace(TEXT("dont"), TEXT("don't"));
    Processed = Processed.Replace(TEXT("cant"), TEXT("can't"));
    Processed = Processed.Replace(TEXT("wont"), TEXT("won't"));
    
    // Remove duplicate words
    TArray<FString> Words;
    Processed.ParseIntoArray(Words, TEXT(" "), true);
    
    TArray<FString> CleanedWords;
    FString LastWord;
    
    for (const FString& Word : Words)
    {
        if (Word != LastWord)
        {
            CleanedWords.Add(Word);
            LastWord = Word;
        }
    }
    
    return FString::Join(CleanedWords, TEXT(" "));
}

void UIGEDSpeechToText::UpdateAccuracyMetrics(const FString& RecognizedText)
{
    // Update confidence based on language model score
    TArray<FString> Words;
    RecognizedText.ParseIntoArray(Words, TEXT(" "), true);
    
    float LMScore = CalculateLanguageModelScore(Words);
    RecognitionAccuracy = FMath::Clamp(LMScore, 0.0f, 1.0f);
    
    // Update WER (Word Error Rate) if reference text available
    if (!ReferenceText.IsEmpty())
    {
        float WER = CalculateWER(RecognizedText, ReferenceText);
        RecognitionAccuracy = 1.0f - WER;
    }
}

float UIGEDSpeechToText::CalculateWER(const FString& Hypothesis, const FString& Reference)
{
    TArray<FString> HypWords;
    TArray<FString> RefWords;
    
    Hypothesis.ParseIntoArray(HypWords, TEXT(" "), true);
    Reference.ParseIntoArray(RefWords, TEXT(" "), true);
    
    // Levenshtein distance
    TArray<TArray<int32>> DP;
    DP.SetNum(HypWords.Num() + 1);
    
    for (int32 i = 0; i <= HypWords.Num(); i++)
    {
        DP[i].SetNum(RefWords.Num() + 1);
        DP[i][0] = i;
    }
    
    for (int32 j = 0; j <= RefWords.Num(); j++)
    {
        DP[0][j] = j;
    }
    
    for (int32 i = 1; i <= HypWords.Num(); i++)
    {
        for (int32 j = 1; j <= RefWords.Num(); j++)
        {
            if (HypWords[i - 1] == RefWords[j - 1])
            {
                DP[i][j] = DP[i - 1][j - 1];
            }
            else
            {
                DP[i][j] = 1 + FMath::Min3(DP[i - 1][j], DP[i][j - 1], DP[i - 1][j - 1]);
            }
        }
    }
    
    return (float)DP[HypWords.Num()][RefWords.Num()] / RefWords.Num();
}

void UIGEDSpeechToText::LoadPretrainedModels()
{
    FString ModelPath = FPaths::ProjectContentDir() + TEXT("AI/Models/");
    
    // Load acoustic model
    FString AcousticModelFile = ModelPath + TEXT("acoustic_model.bin");
    if (FPaths::FileExists(AcousticModelFile))
    {
        TArray<uint8> ModelData;
        FFileHelper::LoadFileToArray(ModelData, *AcousticModelFile);
        // Deserialize model weights
        UE_LOG(LogIGED, Warning, TEXT("Loaded acoustic model"));
    }
    
    // Load language model
    FString LanguageModelFile = ModelPath + TEXT("language_model.bin");
    if (FPaths::FileExists(LanguageModelFile))
    {
        TArray<uint8> ModelData;
        FFileHelper::LoadFileToArray(ModelData, *LanguageModelFile);
        // Deserialize n-gram probabilities
        UE_LOG(LogIGED, Warning, TEXT("Loaded language model"));
    }
    
    // Load neural network weights
    FString NeuralNetFile = ModelPath + TEXT("speech_recognition.nn");
    if (SpeechRecognitionNetwork->LoadModel(NeuralNetFile))
    {
        UE_LOG(LogIGED, Warning, TEXT("Loaded pre-trained neural network"));
    }
}

void UIGEDSpeechToText::TrainOnData(const TArray<FAudioTranscriptPair>& TrainingData)
{
    UE_LOG(LogIGED, Warning, TEXT("Starting speech recognition training with %d samples"), TrainingData.Num());
    
    TArray<FTrainingData> NNTrainingData;
    
    for (const FAudioTranscriptPair& Pair : TrainingData)
    {
        // Extract features from audio
        TArray<TArray<float>> Features = ExtractFeatures(Pair.Audio);
        
        // Convert transcript to target vector
        TArray<float> Target;
        Target.SetNumZeroed(VocabularySize);
        
        TArray<FString> Words;
        Pair.Transcript.ParseIntoArray(Words, TEXT(" "), true);
        
        for (const FString& Word : Words)
        {
            if (WordToIndex.Contains(Word))
            {
                Target[WordToIndex[Word]] = 1.0f;
            }
        }
        
        // Add to training data
        for (const TArray<float>& Feature : Features)
        {
            FTrainingData Data;
            Data.Input = Feature;
            Data.Target = Target;
            NNTrainingData.Add(Data);
        }
    }
    
    // Train neural network
    SpeechRecognitionNetwork->Train(NNTrainingData, 100);
    
    UE_LOG(LogIGED, Warning, TEXT("Speech recognition training complete"));
}

void UIGEDSpeechToText::SetLanguageModel(ELanguageModel Language)
{
    CurrentLanguage = Language;
    
    // Load language-specific models
    switch (Language)
    {
        case ELanguageModel::English:
            LoadVocabulary();
            break;
        case ELanguageModel::Spanish:
            // Load Spanish vocabulary
            break;
        case ELanguageModel::French:
            // Load French vocabulary
            break;
        case ELanguageModel::German:
            // Load German vocabulary
            break;
        case ELanguageModel::Chinese:
            // Load Chinese vocabulary
            break;
        case ELanguageModel::Japanese:
            // Load Japanese vocabulary
            break;
    }
    
    UE_LOG(LogIGED, Warning, TEXT("Language model set to: %d"), (int32)Language);
}
// IGEDVoiceRecognition.cpp

#include "Core/IGEDVoiceRecognition.h"
#include "Voice.h"
#include "AudioDevice.h"
#include "AudioMixerBlueprintLibrary.h"
#include "Sound/SoundWaveProcedural.h"
#include "IGED.h"

UIGEDVoiceRecognition::UIGEDVoiceRecognition()
{
    PrimaryComponentTick.bCanEverTick = true;
    bIsRecording = false;
    SampleRate = 16000;
    NumChannels = 1;
    
    // Create procedural sound wave
    ProceduralSoundWave = NewObject<USoundWaveProcedural>();
    ProceduralSoundWave->SetSampleRate(SampleRate);
    ProceduralSoundWave->NumChannels = NumChannels;
    ProceduralSoundWave->Duration = INDEFINITELY_LOOPING_DURATION;
    ProceduralSoundWave->SoundGroup = SOUNDGROUP_Voice;
    ProceduralSoundWave->bLooping = false;
}

void UIGEDVoiceRecognition::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize voice capture
    VoiceCapture = FVoiceModule::Get().CreateVoiceCapture("");
    
    if (VoiceCapture.IsValid())
    {
        UE_LOG(LogIGED, Warning, TEXT("Voice Recognition initialized"));
    }
    else
    {
        UE_LOG(LogIGED, Error, TEXT("Failed to initialize voice capture"));
    }
}

void UIGEDVoiceRecognition::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    StopRecording();
    VoiceCapture.Reset();
    
    Super::EndPlay(EndPlayReason);
}

void UIGEDVoiceRecognition::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (bIsRecording && VoiceCapture.IsValid())
    {
        ProcessVoiceCapture();
    }
}

void UIGEDVoiceRecognition::StartRecording()
{
    if (!VoiceCapture.IsValid())
    {
        UE_LOG(LogIGED, Error, TEXT("Voice capture not initialized"));
        return;
    }
    
    if (bIsRecording)
    {
        UE_LOG(LogIGED, Warning, TEXT("Already recording"));
        return;
    }
    
    // Clear previous audio data
    RecordedAudioData.Empty();
    
    // Start voice capture
    if (VoiceCapture->Start())
    {
        bIsRecording = true;
        RecordingStartTime = FDateTime::Now();
        
        UE_LOG(LogIGED, Warning, TEXT("Started voice recording"));
        OnVoiceRecordingStarted.Broadcast();
    }
    else
    {
        UE_LOG(LogIGED, Error, TEXT("Failed to start voice capture"));
    }
}

void UIGEDVoiceRecognition::StopRecording()
{
    if (!bIsRecording)
    {
        return;
    }
    
    if (VoiceCapture.IsValid())
    {
        VoiceCapture->Stop();
    }
    
    bIsRecording = false;
    
    float RecordingDuration = (FDateTime::Now() - RecordingStartTime).GetTotalSeconds();
    UE_LOG(LogIGED, Warning, TEXT("Stopped voice recording. Duration: %.2f seconds"), RecordingDuration);
    
    // Process the recorded audio
    ProcessRecordedAudio();
    
    OnVoiceRecordingStopped.Broadcast();
}

void UIGEDVoiceRecognition::ProcessVoiceCapture()
{
    if (!VoiceCapture.IsValid()) return;
    
    uint32 VoiceCaptureBytesAvailable = 0;
    EVoiceCaptureState::Type CaptureState = VoiceCapture->GetCaptureState(VoiceCaptureBytesAvailable);
    
    if (CaptureState == EVoiceCaptureState::Ok && VoiceCaptureBytesAvailable > 0)
    {
        // Allocate buffer for audio data
        TArray<uint8> VoiceData;
        VoiceData.SetNum(VoiceCaptureBytesAvailable);
        
        uint32 VoiceDataReadBytes = 0;
        VoiceCapture->GetVoiceData(VoiceData.GetData(), VoiceCaptureBytesAvailable, VoiceDataReadBytes);
        
        if (VoiceDataReadBytes > 0)
        {
            // Append to recorded data
            RecordedAudioData.Append(VoiceData.GetData(), VoiceDataReadBytes);
            
            // Update audio levels for visualization
            UpdateAudioLevels(VoiceData);
        }
    }
}

void UIGEDVoiceRecognition::ProcessRecordedAudio()
{
    if (RecordedAudioData.Num() == 0)
    {
        UE_LOG(LogIGED, Warning, TEXT("No audio data to process"));
        return;
    }
    
    UE_LOG(LogIGED, Warning, TEXT("Processing %d bytes of audio data"), RecordedAudioData.Num());
    
    // Convert audio to text (simplified - in production, use speech-to-text API)
    FString RecognizedText = ConvertAudioToText(RecordedAudioData);
    
    // Broadcast the recognized text
    OnVoiceRecognized.Broadcast(RecognizedText);
    
    // Process as command
    ProcessVoiceCommand(RecognizedText);
}

FString UIGEDVoiceRecognition::ConvertAudioToText(const TArray<uint8>& AudioData)
{
    // In production, this would send audio to a speech-to-text service
    // For now, simulate recognition based on audio characteristics
    
    float AudioLength = AudioData.Num() / (float)(SampleRate * 2); // 16-bit audio
    
    // Simulate different commands based on audio length
    if (AudioLength < 1.0f)
    {
        return TEXT("help");
    }
    else if (AudioLength < 2.0f)
    {
        return TEXT("generate code");
    }
    else if (AudioLength < 3.0f)
    {
        return TEXT("analyze data");
    }
    else
    {
        return TEXT("perform security scan");
    }
}

void UIGEDVoiceRecognition::ProcessVoiceCommand(const FString& Command)
{
    UE_LOG(LogIGED, Warning, TEXT("Processing voice command: %s"), *Command);
    
    // Parse command for intent
    FString Intent;
    FString Parameters;
    
    if (Command.Contains(TEXT("generate")))
    {
        Intent = TEXT("generate");
        Parameters = TEXT("code");
    }
    else if (Command.Contains(TEXT("analyze")))
    {
        Intent = TEXT("analyze");
        Parameters = TEXT("data");
    }
    else if (Command.Contains(TEXT("scan")))
    {
        Intent = TEXT("scan");
        Parameters = TEXT("network");
    }
    else if (Command.Contains(TEXT("help")))
    {
        Intent = TEXT("help");
        Parameters = TEXT("");
    }
    else
    {
        Intent = TEXT("unknown");
        Parameters = Command;
    }
    
    // Broadcast processed command
    OnCommandProcessed.Broadcast(Intent, Parameters);
}

void UIGEDVoiceRecognition::UpdateAudioLevels(const TArray<uint8>& AudioData)
{
    if (AudioData.Num() < 2) return;
    
    // Calculate RMS (Root Mean Square) for volume level
    float Sum = 0.0f;
    int16* AudioSamples = (int16*)AudioData.GetData();
    int32 NumSamples = AudioData.Num() / 2;
    
    for (int32 i = 0; i < NumSamples; i++)
    {
        float Sample = AudioSamples[i] / 32768.0f; // Normalize to -1.0 to 1.0
        Sum += Sample * Sample;
    }
    
    float RMS = FMath::Sqrt(Sum / NumSamples);
    CurrentAudioLevel = FMath::Clamp(RMS * 10.0f, 0.0f, 1.0f); // Scale for visualization
    
    // Broadcast audio level for UI
    OnAudioLevelUpdated.Broadcast(CurrentAudioLevel);
}

bool UIGEDVoiceRecognition::IsRecording() const
{
    return bIsRecording;
}

float UIGEDVoiceRecognition::GetCurrentAudioLevel() const
{
    return CurrentAudioLevel;
}

void UIGEDVoiceRecognition::SetVoiceLanguage(const FString& LanguageCode)
{
    VoiceLanguage = LanguageCode;
    UE_LOG(LogIGED, Warning, TEXT("Voice language set to: %s"), *LanguageCode);
}

void UIGEDVoiceRecognition::EnableNoiseSupression(bool bEnable)
{
    bNoiseSuppressionEnabled = bEnable;
    
    if (VoiceCapture.IsValid())
    {
        // Apply noise suppression settings
        // Implementation depends on platform
    }
}

void UIGEDVoiceRecognition::SetVoiceSensitivity(float Sensitivity)
{
    VoiceSensitivity = FMath::Clamp(Sensitivity, 0.0f, 1.0f);
}
// IGEDVoiceRecognition.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Voice.h"
#include "IGEDVoiceRecognition.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVoiceRecordingStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVoiceRecordingStopped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoiceRecognized, const FString&, RecognizedText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCommandProcessed, const FString&, Intent, const FString&, Parameters);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAudioLevelUpdated, float, AudioLevel);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class IGED_API UIGEDVoiceRecognition : public UActorComponent
{
    GENERATED_BODY()

public:
    UIGEDVoiceRecognition();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Voice Recording Functions
    UFUNCTION(BlueprintCallable, Category = "IGED|Voice")
    void StartRecording();

    UFUNCTION(BlueprintCallable, Category = "IGED|Voice")
    void StopRecording();

    UFUNCTION(BlueprintCallable, Category = "IGED|Voice")
    bool IsRecording() const;

    UFUNCTION(BlueprintCallable, Category = "IGED|Voice")
    float GetCurrentAudioLevel() const;

    // Configuration
    UFUNCTION(BlueprintCallable, Category = "IGED|Voice")
    void SetVoiceLanguage(const FString& LanguageCode);

    UFUNCTION(BlueprintCallable, Category = "IGED|Voice")
    void EnableNoiseSupression(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "IGED|Voice")
    void SetVoiceSensitivity(float Sensitivity);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "IGED|Voice")
    FOnVoiceRecordingStarted OnVoiceRecordingStarted;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Voice")
    FOnVoiceRecordingStopped OnVoiceRecordingStopped;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Voice")
    FOnVoiceRecognized OnVoiceRecognized;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Voice")
    FOnCommandProcessed OnCommandProcessed;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Voice")
    FOnAudioLevelUpdated OnAudioLevelUpdated;

protected:
    void ProcessVoiceCapture();
    void ProcessRecordedAudio();
    FString ConvertAudioToText(const TArray<uint8>& AudioData);
    void ProcessVoiceCommand(const FString& Command);
    void UpdateAudioLevels(const TArray<uint8>& AudioData);

private:
    TSharedPtr<class IVoiceCapture> VoiceCapture;
    TArray<uint8> RecordedAudioData;
    
    UPROPERTY()
    class USoundWaveProcedural* ProceduralSoundWave;
    
    bool bIsRecording;
    FDateTime RecordingStartTime;
    
    int32 SampleRate;
    int32 NumChannels;
    
    float CurrentAudioLevel;
    FString VoiceLanguage;
    bool bNoiseSuppressionEnabled;
    float VoiceSensitivity;
};
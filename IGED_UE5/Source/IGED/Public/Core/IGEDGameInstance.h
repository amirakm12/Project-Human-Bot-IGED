// IGEDGameInstance.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Http.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "IGEDGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAIResponseReceived, const FString&, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoiceCommandReceived, const FString&, Command);

UCLASS()
class IGED_API UIGEDGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UIGEDGameInstance();

    virtual void Init() override;
    virtual void Shutdown() override;

    // Core Systems
    UFUNCTION(BlueprintCallable, Category = "IGED|Core")
    void InitializeAISystem();

    UFUNCTION(BlueprintCallable, Category = "IGED|Core")
    void InitializeVoiceRecognition();

    UFUNCTION(BlueprintCallable, Category = "IGED|Core")
    void InitializeNetworking();

    // AI Agent Functions
    UFUNCTION(BlueprintCallable, Category = "IGED|AI")
    void ExecuteAICommand(const FString& Command, const FString& Parameters);

    UFUNCTION(BlueprintCallable, Category = "IGED|AI")
    FString ProcessNaturalLanguage(const FString& Input);

    UFUNCTION(BlueprintCallable, Category = "IGED|AI")
    void TrainAIModel(const TArray<FString>& TrainingData);

    // Voice Functions
    UFUNCTION(BlueprintCallable, Category = "IGED|Voice")
    void StartVoiceCapture();

    UFUNCTION(BlueprintCallable, Category = "IGED|Voice")
    void StopVoiceCapture();

    UFUNCTION(BlueprintCallable, Category = "IGED|Voice")
    void ProcessVoiceCommand(const TArray<uint8>& AudioData);

    // Network Functions
    UFUNCTION(BlueprintCallable, Category = "IGED|Network")
    void ConnectToServer(const FString& ServerURL, int32 Port);

    UFUNCTION(BlueprintCallable, Category = "IGED|Network")
    void SendDataToServer(const FString& Data);

    // Security Functions
    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    FString EncryptData(const FString& PlainText);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    FString DecryptData(const FString& EncryptedText);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    bool AuthenticateUser(const FString& Username, const FString& Password);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "IGED|Events")
    FOnAIResponseReceived OnAIResponseReceived;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Events")
    FOnVoiceCommandReceived OnVoiceCommandReceived;

private:
    TSharedPtr<IWebSocket> WebSocket;
    FHttpModule* HttpModule;
    
    // AI System Components
    class UAISubsystem* AISubsystem;
    class UVoiceRecognitionSubsystem* VoiceSubsystem;
    class UNetworkingSubsystem* NetworkSubsystem;
    class USecuritySubsystem* SecuritySubsystem;

    void OnWebSocketConnected();
    void OnWebSocketConnectionError(const FString& Error);
    void OnWebSocketClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
    void OnWebSocketMessage(const FString& Message);
};
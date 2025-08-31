// IGED Collaboration Manager - Real-time Collaboration Features
// Copyright (c) 2024 Project Human Bot

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IGEDCollaborationManager.generated.h"

// Collaboration session structure
USTRUCT(BlueprintType)
struct FCollaborationSession
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    FString SessionID;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    FString SessionName;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    FString HostID;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    TArray<FString> ParticipantIDs;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    FDateTime CreatedTime;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    FDateTime LastActivity;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    bool bIsActive;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    int32 MaxParticipants;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    FString SessionType;

    FCollaborationSession()
        : bIsActive(false)
        , MaxParticipants(10)
    {}
};

// Collaboration participant structure
USTRUCT(BlueprintType)
struct FCollaborationParticipant
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    FString ParticipantID;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    FString ParticipantName;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    FString SessionID;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    FDateTime JoinedTime;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    FDateTime LastSeen;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    bool bIsOnline;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    bool bIsHost;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    FString Role;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    TArray<FString> Permissions;

    FCollaborationParticipant()
        : bIsOnline(false)
        , bIsHost(false)
    {}
};

// Collaboration message structure
USTRUCT(BlueprintType)
struct FCollaborationMessage
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    FString MessageID;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    FString SessionID;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    FString SenderID;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    FString MessageType;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    FString Content;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    FDateTime Timestamp;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    bool bIsRead;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    TArray<FString> RecipientIDs;

    FCollaborationMessage()
        : bIsRead(false)
    {}
};

// Collaboration statistics structure
USTRUCT(BlueprintType)
struct FCollaborationStatistics
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    int32 TotalSessions;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    int32 ActiveSessions;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    int32 TotalParticipants;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    int32 OnlineParticipants;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    int32 TotalMessages;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    float AverageSessionDuration;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    float AverageParticipantsPerSession;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    float MessageDeliveryRate;

    FCollaborationStatistics()
        : TotalSessions(0)
        , ActiveSessions(0)
        , TotalParticipants(0)
        , OnlineParticipants(0)
        , TotalMessages(0)
        , AverageSessionDuration(0.0f)
        , AverageParticipantsPerSession(0.0f)
        , MessageDeliveryRate(0.0f)
    {}
};

// Collaboration configuration structure
USTRUCT(BlueprintType)
struct FCollaborationConfiguration
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    bool bEnableRealTimeMessaging;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    bool bEnableFileSharing;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    bool bEnableScreenSharing;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    bool bEnableVoiceChat;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    bool bEnableVideoChat;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    bool bEnableWhiteboard;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    int32 MaxSessions;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    int32 MaxParticipantsPerSession;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Collaboration")
    float MessageTimeout;

    FCollaborationConfiguration()
        : bEnableRealTimeMessaging(true)
        , bEnableFileSharing(true)
        , bEnableScreenSharing(true)
        , bEnableVoiceChat(true)
        , bEnableVideoChat(true)
        , bEnableWhiteboard(true)
        , MaxSessions(100)
        , MaxParticipantsPerSession(50)
        , MessageTimeout(30.0f)
    {}
};

// Collaboration role enum
UENUM(BlueprintType)
enum class ECollaborationRole : uint8
{
    Viewer UMETA(DisplayName = "Viewer"),
    Participant UMETA(DisplayName = "Participant"),
    Moderator UMETA(DisplayName = "Moderator"),
    Host UMETA(DisplayName = "Host"),
    Admin UMETA(DisplayName = "Admin")
};

// Message type enum
UENUM(BlueprintType)
enum class EMessageType : uint8
{
    Text UMETA(DisplayName = "Text"),
    File UMETA(DisplayName = "File"),
    Image UMETA(DisplayName = "Image"),
    Video UMETA(DisplayName = "Video"),
    Audio UMETA(DisplayName = "Audio"),
    System UMETA(DisplayName = "System"),
    Command UMETA(DisplayName = "Command")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionCreated, const FCollaborationSession&, Session);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnParticipantJoined, const FCollaborationParticipant&, Participant);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnParticipantLeft, const FCollaborationParticipant&, Participant);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageReceived, const FCollaborationMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCollaborationStatisticsUpdated, const FCollaborationStatistics&, Statistics);

UCLASS(Blueprintable)
class IGED_UE5_API UIGEDCollaborationManager : public UObject
{
    GENERATED_BODY()

public:
    UIGEDCollaborationManager();

    // Initialization
    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void Initialize(const FCollaborationConfiguration& Config);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void Shutdown();

    // Session management
    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    FString CreateSession(const FString& SessionName, const FString& SessionType = TEXT("General"));

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    bool JoinSession(const FString& SessionID, const FString& ParticipantName);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    bool LeaveSession(const FString& SessionID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    bool EndSession(const FString& SessionID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    FCollaborationSession GetSession(const FString& SessionID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    TArray<FCollaborationSession> GetAllSessions();

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    TArray<FCollaborationSession> GetActiveSessions();

    // Participant management
    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    FCollaborationParticipant GetParticipant(const FString& ParticipantID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    TArray<FCollaborationParticipant> GetSessionParticipants(const FString& SessionID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    TArray<FCollaborationParticipant> GetOnlineParticipants();

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    bool SetParticipantRole(const FString& ParticipantID, ECollaborationRole Role);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    bool SetParticipantPermissions(const FString& ParticipantID, const TArray<FString>& Permissions);

    // Messaging
    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    FString SendMessage(const FString& SessionID, const FString& Content, EMessageType Type = EMessageType::Text);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    FString SendMessageToParticipant(const FString& ParticipantID, const FString& Content, EMessageType Type = EMessageType::Text);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    TArray<FCollaborationMessage> GetSessionMessages(const FString& SessionID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    TArray<FCollaborationMessage> GetParticipantMessages(const FString& ParticipantID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    bool MarkMessageAsRead(const FString& MessageID);

    // Real-time features
    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void EnableRealTimeMessaging(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void EnableFileSharing(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void EnableScreenSharing(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void EnableVoiceChat(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void EnableVideoChat(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void EnableWhiteboard(bool bEnable);

    // File sharing
    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    FString ShareFile(const FString& SessionID, const FString& FilePath, const FString& FileName);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    bool DownloadFile(const FString& FileID, const FString& DownloadPath);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    TArray<FString> GetSharedFiles(const FString& SessionID);

    // Screen sharing
    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void StartScreenSharing(const FString& SessionID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void StopScreenSharing(const FString& SessionID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    bool IsScreenSharing(const FString& SessionID);

    // Voice and video chat
    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void StartVoiceChat(const FString& SessionID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void StopVoiceChat(const FString& SessionID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void StartVideoChat(const FString& SessionID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void StopVideoChat(const FString& SessionID);

    // Whiteboard
    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void StartWhiteboard(const FString& SessionID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void StopWhiteboard(const FString& SessionID);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void DrawOnWhiteboard(const FString& SessionID, const FString& DrawingData);

    // Statistics and monitoring
    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    FCollaborationStatistics GetStatistics();

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void StartMonitoring();

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void StopMonitoring();

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void UpdateStatistics();

    // Configuration
    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    void SetConfiguration(const FCollaborationConfiguration& Config);

    UFUNCTION(BlueprintCallable, Category = "IGED|Collaboration")
    FCollaborationConfiguration GetConfiguration() const;

    // Events
    UPROPERTY(BlueprintAssignable, Category = "IGED|Collaboration|Events")
    FOnSessionCreated OnSessionCreated;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Collaboration|Events")
    FOnParticipantJoined OnParticipantJoined;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Collaboration|Events")
    FOnParticipantLeft OnParticipantLeft;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Collaboration|Events")
    FOnMessageReceived OnMessageReceived;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Collaboration|Events")
    FOnCollaborationStatisticsUpdated OnCollaborationStatisticsUpdated;

protected:
    UPROPERTY()
    bool bIsInitialized;

    UPROPERTY()
    bool bIsMonitoring;

    UPROPERTY()
    FCollaborationConfiguration Configuration;

    UPROPERTY()
    TMap<FString, FCollaborationSession> Sessions;

    UPROPERTY()
    TMap<FString, FCollaborationParticipant> Participants;

    UPROPERTY()
    TMap<FString, FCollaborationMessage> Messages;

    UPROPERTY()
    FCollaborationStatistics Statistics;

    UPROPERTY()
    TMap<FString, TArray<FString>> SessionParticipants;

    UPROPERTY()
    TMap<FString, TArray<FString>> ParticipantSessions;

private:
    void UpdateStatistics();
    void CleanupInactiveSessions();
    void CleanupInactiveParticipants();
    void ProcessMessageQueue();
    
    // Session management
    void HandleSessionCreated(const FCollaborationSession& Session);
    void HandleSessionEnded(const FString& SessionID);
    void UpdateSessionActivity(const FString& SessionID);
    
    // Participant management
    void HandleParticipantJoined(const FCollaborationParticipant& Participant);
    void HandleParticipantLeft(const FCollaborationParticipant& Participant);
    void UpdateParticipantActivity(const FString& ParticipantID);
    
    // Messaging
    void HandleMessageReceived(const FCollaborationMessage& Message);
    void ProcessMessageDelivery(const FCollaborationMessage& Message);
    void UpdateMessageStatus(const FString& MessageID, bool bIsRead);
    
    // Real-time features
    void HandleRealTimeMessage(const FCollaborationMessage& Message);
    void HandleFileShare(const FString& SessionID, const FString& FileData);
    void HandleScreenShare(const FString& SessionID, const FString& ScreenData);
    void HandleVoiceChat(const FString& SessionID, const FString& AudioData);
    void HandleVideoChat(const FString& SessionID, const FString& VideoData);
    void HandleWhiteboardDraw(const FString& SessionID, const FString& DrawingData);
    
    // Utility functions
    FString GenerateSessionID();
    FString GenerateParticipantID();
    FString GenerateMessageID();
    bool IsParticipantOnline(const FString& ParticipantID);
    bool HasPermission(const FString& ParticipantID, const FString& Permission);
    
    // Timer handles
    FTimerHandle StatisticsTimer;
    FTimerHandle CleanupTimer;
    FTimerHandle MessageProcessingTimer;
    FTimerHandle MonitoringTimer;
};

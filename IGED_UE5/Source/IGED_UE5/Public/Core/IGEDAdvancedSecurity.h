// IGED Advanced Security System - Enhanced Security and Threat Detection
// Copyright (c) 2024 Project Human Bot

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IGEDAdvancedSecurity.generated.h"

// Security threat structure
USTRUCT(BlueprintType)
struct FSecurityThreat
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    FString ThreatID;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    FString ThreatName;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    float Severity;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    FString Source;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    FDateTime DetectedTime;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    bool bIsResolved;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    FString Resolution;

    FSecurityThreat()
        : Severity(0.0f)
        , bIsResolved(false)
    {}
};

// Security event structure
USTRUCT(BlueprintType)
struct FSecurityEvent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    FString EventID;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    FString EventType;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    FString Description;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    FString SourceIP;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    FString UserAgent;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    FDateTime Timestamp;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    float RiskScore;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    bool bIsBlocked;

    FSecurityEvent()
        : RiskScore(0.0f)
        , bIsBlocked(false)
    {}
};

// Security statistics structure
USTRUCT(BlueprintType)
struct FSecurityStatistics
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    int32 TotalThreats;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    int32 ActiveThreats;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    int32 ResolvedThreats;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    int32 TotalEvents;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    int32 BlockedEvents;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    float AverageRiskScore;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    float SecurityScore;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    int32 FailedLoginAttempts;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    int32 SuspiciousActivities;

    FSecurityStatistics()
        : TotalThreats(0)
        , ActiveThreats(0)
        , ResolvedThreats(0)
        , TotalEvents(0)
        , BlockedEvents(0)
        , AverageRiskScore(0.0f)
        , SecurityScore(0.0f)
        , FailedLoginAttempts(0)
        , SuspiciousActivities(0)
    {}
};

// Threat level enum
UENUM(BlueprintType)
enum class EThreatLevel : uint8
{
    Low UMETA(DisplayName = "Low"),
    Medium UMETA(DisplayName = "Medium"),
    High UMETA(DisplayName = "High"),
    Critical UMETA(DisplayName = "Critical")
};

// Security configuration structure
USTRUCT(BlueprintType)
struct FSecurityConfiguration
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    bool bEnableThreatDetection;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    bool bEnableIntrusionDetection;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    bool bEnableMalwareDetection;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    bool bEnableNetworkMonitoring;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    bool bEnableBehavioralAnalysis;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    float ThreatThreshold;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    int32 MaxFailedAttempts;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    float BlockDuration;

    UPROPERTY(BlueprintReadWrite, Category = "IGED|Security")
    bool bEnableAutoResponse;

    FSecurityConfiguration()
        : bEnableThreatDetection(true)
        , bEnableIntrusionDetection(true)
        , bEnableMalwareDetection(true)
        , bEnableNetworkMonitoring(true)
        , bEnableBehavioralAnalysis(true)
        , ThreatThreshold(0.7f)
        , MaxFailedAttempts(5)
        , BlockDuration(300.0f)
        , bEnableAutoResponse(true)
    {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnThreatDetected, const FSecurityThreat&, Threat);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSecurityEvent, const FSecurityEvent&, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnThreatResolved, const FSecurityThreat&, Threat);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSecurityAlert, const FString&, AlertMessage);

UCLASS(Blueprintable)
class IGED_UE5_API UIGEDAdvancedSecurity : public UObject
{
    GENERATED_BODY()

public:
    UIGEDAdvancedSecurity();

    // Initialization
    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void Initialize(const FSecurityConfiguration& Config);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void Shutdown();

    // Threat detection
    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void StartThreatDetection();

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void StopThreatDetection();

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    FSecurityThreat DetectThreat(const FString& Source, const FString& Data);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    bool IsThreatDetected(const FString& Source, const FString& Data);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void ResolveThreat(const FString& ThreatID, const FString& Resolution);

    // Security monitoring
    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void MonitorNetworkActivity();

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void MonitorUserBehavior();

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void MonitorSystemIntegrity();

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void MonitorFileSystem();

    // Event handling
    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void LogSecurityEvent(const FSecurityEvent& Event);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void BlockSuspiciousActivity(const FString& Source);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void UnblockSource(const FString& Source);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    bool IsSourceBlocked(const FString& Source);

    // Statistics and reporting
    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    FSecurityStatistics GetStatistics();

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    TArray<FSecurityThreat> GetActiveThreats();

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    TArray<FSecurityEvent> GetSecurityEvents();

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    FString GenerateSecurityReport();

    // Advanced features
    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void PerformSecurityScan();

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void UpdateThreatDatabase();

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void AnalyzeBehavioralPatterns();

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void ImplementSecurityPolicies();

    // Configuration
    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void SetConfiguration(const FSecurityConfiguration& Config);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    FSecurityConfiguration GetConfiguration() const;

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void SetThreatThreshold(float Threshold);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void SetMaxFailedAttempts(int32 MaxAttempts);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "IGED|Security|Events")
    FOnThreatDetected OnThreatDetected;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Security|Events")
    FOnSecurityEvent OnSecurityEvent;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Security|Events")
    FOnThreatResolved OnThreatResolved;

    UPROPERTY(BlueprintAssignable, Category = "IGED|Security|Events")
    FOnSecurityAlert OnSecurityAlert;

protected:
    UPROPERTY()
    bool bIsInitialized;

    UPROPERTY()
    bool bIsMonitoring;

    UPROPERTY()
    FSecurityConfiguration Configuration;

    UPROPERTY()
    TArray<FSecurityThreat> Threats;

    UPROPERTY()
    TArray<FSecurityEvent> SecurityEvents;

    UPROPERTY()
    TArray<FString> BlockedSources;

    UPROPERTY()
    FSecurityStatistics Statistics;

    UPROPERTY()
    TMap<FString, int32> FailedAttempts;

    UPROPERTY()
    TMap<FString, FDateTime> BlockedUntil;

private:
    void UpdateStatistics();
    void CleanupOldEvents();
    void AnalyzeThreatPatterns();
    void UpdateThreatDatabase();
    
    // Threat detection methods
    bool DetectMalware(const FString& Data);
    bool DetectIntrusion(const FString& Source);
    bool DetectSuspiciousBehavior(const FString& Source);
    bool DetectNetworkAnomalies();
    
    // Security analysis
    float CalculateRiskScore(const FSecurityEvent& Event);
    EThreatLevel DetermineThreatLevel(float Severity);
    void GenerateSecurityAlert(const FString& Message);
    
    // Utility functions
    bool IsValidSource(const FString& Source);
    void LogSecurityEventInternal(const FSecurityEvent& Event);
    void UpdateFailedAttempts(const FString& Source);
    bool IsSourceTemporarilyBlocked(const FString& Source);
    
    // Timer handles
    FTimerHandle ThreatDetectionTimer;
    FTimerHandle SecurityMonitoringTimer;
    FTimerHandle StatisticsTimer;
    FTimerHandle CleanupTimer;
};

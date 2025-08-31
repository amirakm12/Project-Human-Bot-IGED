#pragma once

#include "CoreMinimal.h"
#include "Agents/IGEDBaseAgent.h"
#include "Core/IGEDModelManager.h"
#include "Http.h"
#include "Json.h"
#include "IGEDNetworkIntelligenceAgent.generated.h"

UENUM(BlueprintType)
enum class ENetworkScanType : uint8
{
    PortScan        UMETA(DisplayName = "Port Scan"),
    VulnerabilityScan UMETA(DisplayName = "Vulnerability Scan"),
    ServiceDetection UMETA(DisplayName = "Service Detection"),
    OSFingerprinting UMETA(DisplayName = "OS Fingerprinting"),
    NetworkMapping   UMETA(DisplayName = "Network Mapping"),
    TrafficAnalysis  UMETA(DisplayName = "Traffic Analysis"),
    DNSEnumeration   UMETA(DisplayName = "DNS Enumeration"),
    SubdomainScan    UMETA(DisplayName = "Subdomain Scan"),
    SSLScan          UMETA(DisplayName = "SSL/TLS Scan"),
    WebAppScan       UMETA(DisplayName = "Web Application Scan")
};

UENUM(BlueprintType)
enum class EThreatLevel : uint8
{
    Low             UMETA(DisplayName = "Low"),
    Medium          UMETA(DisplayName = "Medium"),
    High            UMETA(DisplayName = "High"),
    Critical        UMETA(DisplayName = "Critical"),
    Unknown         UMETA(DisplayName = "Unknown")
};

UENUM(BlueprintType)
enum class EProtocolType : uint8
{
    TCP             UMETA(DisplayName = "TCP"),
    UDP             UMETA(DisplayName = "UDP"),
    ICMP            UMETA(DisplayName = "ICMP"),
    HTTP            UMETA(DisplayName = "HTTP"),
    HTTPS           UMETA(DisplayName = "HTTPS"),
    FTP             UMETA(DisplayName = "FTP"),
    SSH             UMETA(DisplayName = "SSH"),
    Telnet          UMETA(DisplayName = "Telnet"),
    SMTP            UMETA(DisplayName = "SMTP"),
    DNS             UMETA(DisplayName = "DNS"),
    DHCP            UMETA(DisplayName = "DHCP"),
    SNMP            UMETA(DisplayName = "SNMP"),
    Custom          UMETA(DisplayName = "Custom")
};

USTRUCT(BlueprintType)
struct FIGEDNetworkTarget
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString TargetID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString IPAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Hostname;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Domain;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> Ports;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Services;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString OperatingSystem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, FString> Metadata;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime LastScanned;

    FIGEDNetworkTarget()
    {
        TargetID = FGuid::NewGuid().ToString();
        IPAddress = TEXT("");
        Hostname = TEXT("");
        Domain = TEXT("");
        OperatingSystem = TEXT("");
        LastScanned = FDateTime::Now();
    }
};

USTRUCT(BlueprintType)
struct FIGEDVulnerability
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString VulnerabilityID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CVE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EThreatLevel ThreatLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CVSSScore;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> AffectedSystems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> ExploitReferences;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> MitigationSteps;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime DiscoveredDate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bExploitable;

    FIGEDVulnerability()
    {
        VulnerabilityID = FGuid::NewGuid().ToString();
        CVE = TEXT("");
        Title = TEXT("");
        Description = TEXT("");
        ThreatLevel = EThreatLevel::Unknown;
        CVSSScore = 0.0f;
        bExploitable = false;
        DiscoveredDate = FDateTime::Now();
    }
};

USTRUCT(BlueprintType)
struct FIGEDNetworkScanRequest
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString RequestID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> TargetIPs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> TargetDomains;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ENetworkScanType ScanType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> Ports;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, FString> Parameters;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime Timestamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bDeepScan;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TimeoutSeconds;

    FIGEDNetworkScanRequest()
    {
        RequestID = FGuid::NewGuid().ToString();
        ScanType = ENetworkScanType::PortScan;
        Timestamp = FDateTime::Now();
        bDeepScan = false;
        TimeoutSeconds = 30;
    }
};

USTRUCT(BlueprintType)
struct FIGEDNetworkScanResult
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString RequestID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FIGEDNetworkTarget> DiscoveredTargets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FIGEDVulnerability> Vulnerabilities;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> OpenPorts;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Services;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString NetworkTopology;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Recommendations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, float> RiskMetrics;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime ScanTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSuccess;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ErrorMessage;

    FIGEDNetworkScanResult()
    {
        RequestID = TEXT("");
        NetworkTopology = TEXT("");
        ScanTime = FDateTime::Now();
        bSuccess = false;
        ErrorMessage = TEXT("");
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNetworkScanCompleted, const FString&, RequestID, const FIGEDNetworkScanResult&, Result);

UCLASS(BlueprintType, Blueprintable)
class IGED_UE5_API UIGEDNetworkIntelligenceAgent : public UIGEDBaseAgent
{
    GENERATED_BODY()

public:
    UIGEDNetworkIntelligenceAgent();

    // UIGEDBaseAgent interface
    virtual bool ExecuteTask(const FString& TaskDescription, const TMap<FString, FString>& Parameters) override;
    virtual FString GetAgentName() const override;
    virtual FString GetStatus() const override;

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    bool Initialize(UIGEDModelManager* ModelManager);

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    void Shutdown();

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    FString ScanNetwork(const FIGEDNetworkScanRequest& Request);

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    bool ScanNetworkAsync(const FIGEDNetworkScanRequest& Request);

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    TArray<FIGEDNetworkTarget> DiscoverHosts(const FString& NetworkRange);

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    TArray<int32> ScanPorts(const FString& IPAddress, const TArray<int32>& Ports);

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    TArray<FString> DetectServices(const FString& IPAddress, const TArray<int32>& OpenPorts);

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    FString FingerprintOS(const FString& IPAddress);

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    TArray<FIGEDVulnerability> ScanVulnerabilities(const FString& IPAddress, const TArray<FString>& Services);

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    FString AnalyzeTraffic(const TArray<FString>& TrafficData);

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    TArray<FString> EnumerateDNS(const FString& Domain);

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    TArray<FString> FindSubdomains(const FString& Domain);

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    TArray<FString> ScanSSL(const FString& Hostname, int32 Port = 443);

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    TArray<FString> ScanWebApplication(const FString& URL);

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    TArray<FString> GenerateSecurityRecommendations(const FIGEDNetworkScanResult& ScanResult);

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    TMap<FString, float> CalculateRiskMetrics(const TArray<FIGEDVulnerability>& Vulnerabilities);

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    bool StartContinuousMonitoring(const FString& NetworkRange, const TArray<ENetworkScanType>& ScanTypes);

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    void StopContinuousMonitoring(const FString& NetworkRange);

    UFUNCTION(BlueprintCallable, Category = "IGED Network Intelligence")
    TArray<FString> GetThreatIntelligence(const FString& IPAddress);

    UPROPERTY(BlueprintAssignable, Category = "IGED Network Intelligence")
    FOnNetworkScanCompleted OnNetworkScanCompleted;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    UIGEDModelManager* ModelManager;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, FIGEDNetworkScanRequest> ActiveScans;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, FIGEDNetworkScanResult> ScanResults;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, bool> ContinuousMonitoring;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    FString ActiveModelName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TArray<FString> KnownVulnerabilities;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, TArray<FString>> ThreatIntelligenceDB;

private:
    FCriticalSection ScanMutex;
    FCriticalSection MonitoringMutex;

    void ProcessNetworkScan(const FString& RequestID);
    void HandleModelResponse(const FString& ModelName, const FString& Response, bool bSuccess);
    void InitializeVulnerabilityDatabase();
    void InitializeThreatIntelligence();
    FString FormatScanPrompt(const FIGEDNetworkScanRequest& Request);
    TArray<FIGEDNetworkTarget> ParseTargetsFromResponse(const FString& Response);
    TArray<FIGEDVulnerability> ParseVulnerabilitiesFromResponse(const FString& Response);
    TArray<FString> ParseRecommendationsFromResponse(const FString& Response);
    TMap<FString, float> ParseRiskMetricsFromResponse(const FString& Response);
    bool ValidateScanRequest(const FIGEDNetworkScanRequest& Request);
    void ProcessContinuousMonitoring();
    TArray<FString> SimulateNetworkScan(const FIGEDNetworkScanRequest& Request);
    TArray<FIGEDVulnerability> SimulateVulnerabilityScan(const FString& IPAddress);
    FString GenerateNetworkTopology(const TArray<FIGEDNetworkTarget>& Targets);
};

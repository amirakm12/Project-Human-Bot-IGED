#pragma once

#include "CoreMinimal.h"
#include "Agents/IGEDBaseAgent.h"
#include "Core/IGEDModelManager.h"
#include "Http.h"
#include "Json.h"
#include "IGEDRemoteControlAgent.generated.h"

UENUM(BlueprintType)
enum class ERemoteControlType : uint8
{
    SystemCommand   UMETA(DisplayName = "System Command"),
    ProcessControl  UMETA(DisplayName = "Process Control"),
    FileOperation   UMETA(DisplayName = "File Operation"),
    NetworkControl  UMETA(DisplayName = "Network Control"),
    ServiceControl  UMETA(DisplayName = "Service Control"),
    RegistryControl UMETA(DisplayName = "Registry Control"),
    DeviceControl   UMETA(DisplayName = "Device Control"),
    ApplicationControl UMETA(DisplayName = "Application Control"),
    SecurityControl UMETA(DisplayName = "Security Control"),
    CustomCommand   UMETA(DisplayName = "Custom Command")
};

UENUM(BlueprintType)
enum class EExecutionResult : uint8
{
    Success         UMETA(DisplayName = "Success"),
    Failed          UMETA(DisplayName = "Failed"),
    Timeout         UMETA(DisplayName = "Timeout"),
    PermissionDenied UMETA(DisplayName = "Permission Denied"),
    InvalidCommand  UMETA(DisplayName = "Invalid Command"),
    SystemError     UMETA(DisplayName = "System Error"),
    NetworkError    UMETA(DisplayName = "Network Error"),
    Unknown         UMETA(DisplayName = "Unknown")
};

UENUM(BlueprintType)
enum class EPlatformType : uint8
{
    Windows         UMETA(DisplayName = "Windows"),
    Linux           UMETA(DisplayName = "Linux"),
    macOS           UMETA(DisplayName = "macOS"),
    Android         UMETA(DisplayName = "Android"),
    iOS             UMETA(DisplayName = "iOS"),
    CrossPlatform   UMETA(DisplayName = "Cross Platform"),
    Unknown         UMETA(DisplayName = "Unknown")
};

USTRUCT(BlueprintType)
struct FIGEDRemoteCommand
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CommandID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Command;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ERemoteControlType CommandType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Arguments;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, FString> Parameters;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EPlatformType TargetPlatform;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString TargetSystem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TimeoutSeconds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bRequireConfirmation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime Timestamp;

    FIGEDRemoteCommand()
    {
        CommandID = FGuid::NewGuid().ToString();
        Command = TEXT("");
        CommandType = ERemoteControlType::SystemCommand;
        TargetPlatform = EPlatformType::Windows;
        TargetSystem = TEXT("");
        TimeoutSeconds = 30;
        bRequireConfirmation = false;
        Timestamp = FDateTime::Now();
    }
};

USTRUCT(BlueprintType)
struct FIGEDRemoteExecutionResult
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CommandID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EExecutionResult Result;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Output;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ErrorOutput;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ExitCode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ExecutionTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString TargetSystem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, FString> Metadata;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime CompletionTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSuccess;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ErrorMessage;

    FIGEDRemoteExecutionResult()
    {
        CommandID = TEXT("");
        Result = EExecutionResult::Unknown;
        Output = TEXT("");
        ErrorOutput = TEXT("");
        ExitCode = -1;
        ExecutionTime = 0.0f;
        TargetSystem = TEXT("");
        CompletionTime = FDateTime::Now();
        bSuccess = false;
        ErrorMessage = TEXT("");
    }
};

USTRUCT(BlueprintType)
struct FIGEDRemoteSystem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString SystemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString IPAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EPlatformType Platform;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString OperatingSystem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Architecture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, FString> ConnectionInfo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bOnline;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime LastSeen;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Capabilities;

    FIGEDRemoteSystem()
    {
        SystemID = FGuid::NewGuid().ToString();
        Name = TEXT("");
        IPAddress = TEXT("");
        Platform = EPlatformType::Unknown;
        OperatingSystem = TEXT("");
        Architecture = TEXT("");
        bOnline = false;
        LastSeen = FDateTime::Now();
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRemoteCommandCompleted, const FString&, CommandID, const FIGEDRemoteExecutionResult&, Result);

UCLASS(BlueprintType, Blueprintable)
class IGED_UE5_API UIGEDRemoteControlAgent : public UIGEDBaseAgent
{
    GENERATED_BODY()

public:
    UIGEDRemoteControlAgent();

    // UIGEDBaseAgent interface
    virtual bool ExecuteTask(const FString& TaskDescription, const TMap<FString, FString>& Parameters) override;
    virtual FString GetAgentName() const override;
    virtual FString GetStatus() const override;

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool Initialize(UIGEDModelManager* ModelManager);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    void Shutdown();

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    FString ExecuteRemoteCommand(const FIGEDRemoteCommand& Command);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool ExecuteRemoteCommandAsync(const FIGEDRemoteCommand& Command);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool AddRemoteSystem(const FIGEDRemoteSystem& System);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool RemoveRemoteSystem(const FString& SystemID);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool UpdateRemoteSystem(const FString& SystemID, const FIGEDRemoteSystem& UpdatedSystem);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    TArray<FIGEDRemoteSystem> GetRemoteSystems() const;

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool TestConnection(const FString& SystemID);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    FString ExecuteSystemCommand(const FString& SystemID, const FString& Command, const TArray<FString>& Arguments);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool StartProcess(const FString& SystemID, const FString& ProcessPath, const TArray<FString>& Arguments);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool StopProcess(const FString& SystemID, const FString& ProcessName);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    TArray<FString> ListProcesses(const FString& SystemID);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool CreateFile(const FString& SystemID, const FString& FilePath, const FString& Content);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool DeleteFile(const FString& SystemID, const FString& FilePath);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    FString ReadFile(const FString& SystemID, const FString& FilePath);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool CopyFile(const FString& SystemID, const FString& SourcePath, const FString& DestinationPath);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    TArray<FString> ListDirectory(const FString& SystemID, const FString& DirectoryPath);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool StartService(const FString& SystemID, const FString& ServiceName);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool StopService(const FString& SystemID, const FString& ServiceName);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    TArray<FString> ListServices(const FString& SystemID);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool SetRegistryValue(const FString& SystemID, const FString& KeyPath, const FString& ValueName, const FString& Value);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    FString GetRegistryValue(const FString& SystemID, const FString& KeyPath, const FString& ValueName);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool DeleteRegistryValue(const FString& SystemID, const FString& KeyPath, const FString& ValueName);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    TArray<FString> GetSystemInfo(const FString& SystemID);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool InstallSoftware(const FString& SystemID, const FString& SoftwarePath, const TArray<FString>& InstallArgs);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool UninstallSoftware(const FString& SystemID, const FString& SoftwareName);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    TArray<FString> ListInstalledSoftware(const FString& SystemID);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool RebootSystem(const FString& SystemID);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool ShutdownSystem(const FString& SystemID);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    bool StartContinuousMonitoring(const FString& SystemID);

    UFUNCTION(BlueprintCallable, Category = "IGED Remote Control")
    void StopContinuousMonitoring(const FString& SystemID);

    UPROPERTY(BlueprintAssignable, Category = "IGED Remote Control")
    FOnRemoteCommandCompleted OnRemoteCommandCompleted;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    UIGEDModelManager* ModelManager;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, FIGEDRemoteSystem> RemoteSystems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, FIGEDRemoteCommand> ActiveCommands;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, FIGEDRemoteExecutionResult> CommandResults;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, bool> ContinuousMonitoring;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    FString ActiveModelName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, TArray<FString>> CommandTemplates;

private:
    FCriticalSection SystemMutex;
    FCriticalSection CommandMutex;

    void ProcessRemoteCommand(const FString& CommandID);
    void HandleModelResponse(const FString& ModelName, const FString& Response, bool bSuccess);
    bool ValidateRemoteSystem(const FIGEDRemoteSystem& System);
    bool ValidateRemoteCommand(const FIGEDRemoteCommand& Command);
    void InitializeCommandTemplates();
    FString FormatCommandPrompt(const FIGEDRemoteCommand& Command);
    FIGEDRemoteExecutionResult ParseExecutionResult(const FString& Response);
    void ProcessContinuousMonitoring();
    bool ExecuteLocalCommand(const FString& Command, const TArray<FString>& Arguments, FString& Output, FString& ErrorOutput);
    bool ExecuteRemoteCommandViaSSH(const FIGEDRemoteSystem& System, const FString& Command, const TArray<FString>& Arguments, FString& Output, FString& ErrorOutput);
    bool ExecuteRemoteCommandViaHTTP(const FIGEDRemoteSystem& System, const FString& Command, const TArray<FString>& Arguments, FString& Output, FString& ErrorOutput);
    TArray<FString> GetSystemCapabilities(const FString& SystemID);
    void UpdateSystemStatus(const FString& SystemID, bool bOnline);
    FString GenerateCommandFromDescription(const FString& Description, ERemoteControlType CommandType, EPlatformType Platform);
};

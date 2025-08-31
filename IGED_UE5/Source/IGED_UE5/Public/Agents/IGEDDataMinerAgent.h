#pragma once

#include "CoreMinimal.h"
#include "Agents/IGEDBaseAgent.h"
#include "Core/IGEDModelManager.h"
#include "Engine/DataTable.h"
#include "Http.h"
#include "Json.h"
#include "IGEDDataMinerAgent.generated.h"

UENUM(BlueprintType)
enum class EDataSourceType : uint8
{
    Database       UMETA(DisplayName = "Database"),
    API            UMETA(DisplayName = "API"),
    File           UMETA(DisplayName = "File"),
    WebScraping    UMETA(DisplayName = "Web Scraping"),
    Stream         UMETA(DisplayName = "Data Stream"),
    Cloud          UMETA(DisplayName = "Cloud Storage"),
    SocialMedia    UMETA(DisplayName = "Social Media"),
    IoT            UMETA(DisplayName = "IoT Devices"),
    Blockchain     UMETA(DisplayName = "Blockchain"),
    Custom         UMETA(DisplayName = "Custom Source")
};

UENUM(BlueprintType)
enum class EDataFormat : uint8
{
    JSON           UMETA(DisplayName = "JSON"),
    XML            UMETA(DisplayName = "XML"),
    CSV            UMETA(DisplayName = "CSV"),
    Excel          UMETA(DisplayName = "Excel"),
    Parquet        UMETA(DisplayName = "Parquet"),
    Avro           UMETA(DisplayName = "Avro"),
    ProtocolBuffer UMETA(DisplayName = "Protocol Buffer"),
    Binary         UMETA(DisplayName = "Binary"),
    Text           UMETA(DisplayName = "Text"),
    HTML           UMETA(DisplayName = "HTML"),
    PDF            UMETA(DisplayName = "PDF"),
    Image          UMETA(DisplayName = "Image"),
    Audio          UMETA(DisplayName = "Audio"),
    Video          UMETA(DisplayName = "Video")
};

UENUM(BlueprintType)
enum class EAnalysisType : uint8
{
    Descriptive    UMETA(DisplayName = "Descriptive Analysis"),
    Predictive     UMETA(DisplayName = "Predictive Analysis"),
    Prescriptive   UMETA(DisplayName = "Prescriptive Analysis"),
    Diagnostic     UMETA(DisplayName = "Diagnostic Analysis"),
    Statistical    UMETA(DisplayName = "Statistical Analysis"),
    MachineLearning UMETA(DisplayName = "Machine Learning"),
    DeepLearning   UMETA(DisplayName = "Deep Learning"),
    NLP            UMETA(DisplayName = "Natural Language Processing"),
    ComputerVision UMETA(DisplayName = "Computer Vision"),
    TimeSeries     UMETA(DisplayName = "Time Series Analysis"),
    Clustering     UMETA(DisplayName = "Clustering"),
    Classification UMETA(DisplayName = "Classification"),
    Regression     UMETA(DisplayName = "Regression"),
    AnomalyDetection UMETA(DisplayName = "Anomaly Detection"),
    SentimentAnalysis UMETA(DisplayName = "Sentiment Analysis")
};

USTRUCT(BlueprintType)
struct FIGEDDataSource
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString SourceID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDataSourceType SourceType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ConnectionString;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, FString> Configuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime LastAccessed;

    FIGEDDataSource()
    {
        SourceID = FGuid::NewGuid().ToString();
        Name = TEXT("");
        SourceType = EDataSourceType::Database;
        ConnectionString = TEXT("");
        bEnabled = true;
        Description = TEXT("");
        LastAccessed = FDateTime::Now();
    }
};

USTRUCT(BlueprintType)
struct FIGEDDataRequest
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString RequestID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Query;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> SourceIDs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAnalysisType AnalysisType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, FString> Parameters;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime Timestamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxResults;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bRealTime;

    FIGEDDataRequest()
    {
        RequestID = FGuid::NewGuid().ToString();
        Query = TEXT("");
        AnalysisType = EAnalysisType::Descriptive;
        Timestamp = FDateTime::Now();
        MaxResults = 1000;
        bRealTime = false;
    }
};

USTRUCT(BlueprintType)
struct FIGEDDataResult
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString RequestID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Data;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Analysis;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Insights;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Recommendations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, float> Metrics;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString VisualizationData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime ProcessingTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSuccess;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ErrorMessage;

    FIGEDDataResult()
    {
        RequestID = TEXT("");
        Analysis = TEXT("");
        ProcessingTime = FDateTime::Now();
        bSuccess = false;
        ErrorMessage = TEXT("");
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDataMined, const FString&, RequestID, const FIGEDDataResult&, Result);

UCLASS(BlueprintType, Blueprintable)
class IGED_UE5_API UIGEDDataMinerAgent : public UIGEDBaseAgent
{
    GENERATED_BODY()

public:
    UIGEDDataMinerAgent();

    // UIGEDBaseAgent interface
    virtual bool ExecuteTask(const FString& TaskDescription, const TMap<FString, FString>& Parameters) override;
    virtual FString GetAgentName() const override;
    virtual FString GetStatus() const override;

    UFUNCTION(BlueprintCallable, Category = "IGED Data Mining")
    bool Initialize(UIGEDModelManager* ModelManager);

    UFUNCTION(BlueprintCallable, Category = "IGED Data Mining")
    void Shutdown();

    UFUNCTION(BlueprintCallable, Category = "IGED Data Mining")
    FString MineData(const FIGEDDataRequest& Request);

    UFUNCTION(BlueprintCallable, Category = "IGED Data Mining")
    bool MineDataAsync(const FIGEDDataRequest& Request);

    UFUNCTION(BlueprintCallable, Category = "IGED Data Mining")
    bool AddDataSource(const FIGEDDataSource& DataSource);

    UFUNCTION(BlueprintCallable, Category = "IGED Data Mining")
    bool RemoveDataSource(const FString& SourceID);

    UFUNCTION(BlueprintCallable, Category = "IGED Data Mining")
    bool UpdateDataSource(const FString& SourceID, const FIGEDDataSource& UpdatedSource);

    UFUNCTION(BlueprintCallable, Category = "IGED Data Mining")
    TArray<FIGEDDataSource> GetDataSources() const;

    UFUNCTION(BlueprintCallable, Category = "IGED Data Mining")
    bool TestDataSourceConnection(const FString& SourceID);

    UFUNCTION(BlueprintCallable, Category = "IGED Data Mining")
    FString AnalyzeData(const TArray<FString>& Data, EAnalysisType AnalysisType, const TMap<FString, FString>& Parameters);

    UFUNCTION(BlueprintCallable, Category = "IGED Data Mining")
    TArray<FString> ExtractInsights(const FString& Data, EAnalysisType AnalysisType);

    UFUNCTION(BlueprintCallable, Category = "IGED Data Mining")
    TArray<FString> GenerateRecommendations(const FString& Analysis, const TArray<FString>& Insights);

    UFUNCTION(BlueprintCallable, Category = "IGED Data Mining")
    FString CreateVisualization(const TArray<FString>& Data, const FString& VisualizationType);

    UFUNCTION(BlueprintCallable, Category = "IGED Data Mining")
    bool ValidateData(const TArray<FString>& Data, EDataFormat Format);

    UFUNCTION(BlueprintCallable, Category = "IGED Data Mining")
    TArray<FString> CleanData(const TArray<FString>& Data, const TArray<FString>& CleaningRules);

    UFUNCTION(BlueprintCallable, Category = "IGED Data Mining")
    TMap<FString, float> CalculateMetrics(const TArray<FString>& Data, const TArray<FString>& MetricTypes);

    UFUNCTION(BlueprintCallable, Category = "IGED Data Mining")
    bool StartRealTimeMonitoring(const FString& SourceID, const FString& Query);

    UFUNCTION(BlueprintCallable, Category = "IGED Data Mining")
    void StopRealTimeMonitoring(const FString& SourceID);

    UPROPERTY(BlueprintAssignable, Category = "IGED Data Mining")
    FOnDataMined OnDataMined;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    UIGEDModelManager* ModelManager;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, FIGEDDataSource> DataSources;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, FIGEDDataRequest> ActiveRequests;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, FIGEDDataResult> RequestResults;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    FString ActiveModelName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, bool> RealTimeMonitoring;

private:
    FCriticalSection DataSourceMutex;
    FCriticalSection RequestMutex;

    TArray<FString> FetchDataFromSource(const FIGEDDataSource& Source, const FString& Query);
    FString ProcessDataWithModel(const TArray<FString>& Data, EAnalysisType AnalysisType, const TMap<FString, FString>& Parameters);
    void HandleModelResponse(const FString& ModelName, const FString& Response, bool bSuccess);
    bool ValidateDataSource(const FIGEDDataSource& Source);
    void InitializeDefaultDataSources();
    FString FormatAnalysisPrompt(const TArray<FString>& Data, EAnalysisType AnalysisType, const TMap<FString, FString>& Parameters);
    TArray<FString> ParseDataResponse(const FString& Response);
    TArray<FString> ExtractInsightsFromResponse(const FString& Response);
    TArray<FString> ExtractRecommendationsFromResponse(const FString& Response);
    TMap<FString, float> ExtractMetricsFromResponse(const FString& Response);
    void ProcessRealTimeData(const FString& SourceID, const FString& Data);
};

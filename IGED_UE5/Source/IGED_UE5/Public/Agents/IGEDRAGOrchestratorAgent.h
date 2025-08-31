#pragma once

#include "CoreMinimal.h"
#include "Agents/IGEDBaseAgent.h"
#include "Core/IGEDModelManager.h"
#include "Engine/DataTable.h"
#include "Http.h"
#include "Json.h"
#include "IGEDRAGOrchestratorAgent.generated.h"

UENUM(BlueprintType)
enum class ERAGTaskType : uint8
{
    InformationRetrieval    UMETA(DisplayName = "Information Retrieval"),
    QuestionAnswering       UMETA(DisplayName = "Question Answering"),
    DocumentAnalysis        UMETA(DisplayName = "Document Analysis"),
    CodeGeneration          UMETA(DisplayName = "Code Generation"),
    ResearchSynthesis       UMETA(DisplayName = "Research Synthesis"),
    KnowledgeExtraction     UMETA(DisplayName = "Knowledge Extraction"),
    ContextualReasoning     UMETA(DisplayName = "Contextual Reasoning"),
    MultiModalAnalysis      UMETA(DisplayName = "Multi-Modal Analysis")
};

UENUM(BlueprintType)
enum class ERAGStatus : uint8
{
    Idle            UMETA(DisplayName = "Idle"),
    Retrieving      UMETA(DisplayName = "Retrieving"),
    Processing      UMETA(DisplayName = "Processing"),
    Generating      UMETA(DisplayName = "Generating"),
    Completed       UMETA(DisplayName = "Completed"),
    Error           UMETA(DisplayName = "Error")
};

USTRUCT(BlueprintType)
struct FIGEDRAGQuery
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString QueryID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString UserQuery;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ERAGTaskType TaskType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> ContextDocuments;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, FString> Parameters;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime Timestamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxResults;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SimilarityThreshold;

    FIGEDRAGQuery()
    {
        QueryID = FGuid::NewGuid().ToString();
        UserQuery = TEXT("");
        TaskType = ERAGTaskType::QuestionAnswering;
        MaxResults = 10;
        SimilarityThreshold = 0.7f;
        Timestamp = FDateTime::Now();
    }
};

USTRUCT(BlueprintType)
struct FIGEDRAGResult
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString QueryID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString GeneratedResponse;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> RetrievedDocuments;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<float> SimilarityScores;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Sources;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Confidence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime ProcessingTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ERAGStatus Status;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ErrorMessage;

    FIGEDRAGResult()
    {
        QueryID = TEXT("");
        GeneratedResponse = TEXT("");
        Confidence = 0.0f;
        ProcessingTime = FDateTime::Now();
        Status = ERAGStatus::Idle;
        ErrorMessage = TEXT("");
    }
};

USTRUCT(BlueprintType)
struct FIGEDVectorDocument
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString DocumentID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Content;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<float> VectorEmbedding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, FString> Metadata;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime LastUpdated;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Source;

    FIGEDVectorDocument()
    {
        DocumentID = FGuid::NewGuid().ToString();
        Title = TEXT("");
        Content = TEXT("");
        LastUpdated = FDateTime::Now();
        Source = TEXT("");
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRAGQueryCompleted, const FString&, QueryID, const FIGEDRAGResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRAGStatusChanged, const FString&, QueryID, ERAGStatus, OldStatus, ERAGStatus, NewStatus);

UCLASS(BlueprintType, Blueprintable)
class IGED_UE5_API UIGEDRAGOrchestratorAgent : public UIGEDBaseAgent
{
    GENERATED_BODY()

public:
    UIGEDRAGOrchestratorAgent();

    // UIGEDBaseAgent interface
    virtual bool ExecuteTask(const FString& TaskDescription, const TMap<FString, FString>& Parameters) override;
    virtual FString GetAgentName() const override;
    virtual FString GetStatus() const override;

    UFUNCTION(BlueprintCallable, Category = "IGED RAG Orchestrator")
    bool Initialize(UIGEDModelManager* ModelManager);

    UFUNCTION(BlueprintCallable, Category = "IGED RAG Orchestrator")
    void Shutdown();

    UFUNCTION(BlueprintCallable, Category = "IGED RAG Orchestrator")
    FString ProcessQuery(const FIGEDRAGQuery& Query);

    UFUNCTION(BlueprintCallable, Category = "IGED RAG Orchestrator")
    bool ProcessQueryAsync(const FIGEDRAGQuery& Query);

    UFUNCTION(BlueprintCallable, Category = "IGED RAG Orchestrator")
    bool AddDocument(const FIGEDVectorDocument& Document);

    UFUNCTION(BlueprintCallable, Category = "IGED RAG Orchestrator")
    bool RemoveDocument(const FString& DocumentID);

    UFUNCTION(BlueprintCallable, Category = "IGED RAG Orchestrator")
    bool UpdateDocument(const FString& DocumentID, const FIGEDVectorDocument& UpdatedDocument);

    UFUNCTION(BlueprintCallable, Category = "IGED RAG Orchestrator")
    TArray<FIGEDVectorDocument> SearchDocuments(const FString& Query, int32 MaxResults = 10, float SimilarityThreshold = 0.7f);

    UFUNCTION(BlueprintCallable, Category = "IGED RAG Orchestrator")
    bool GenerateEmbedding(const FString& Text, TArray<float>& OutEmbedding);

    UFUNCTION(BlueprintCallable, Category = "IGED RAG Orchestrator")
    float CalculateSimilarity(const TArray<float>& Embedding1, const TArray<float>& Embedding2);

    UFUNCTION(BlueprintCallable, Category = "IGED RAG Orchestrator")
    bool LoadKnowledgeBase(const FString& FilePath);

    UFUNCTION(BlueprintCallable, Category = "IGED RAG Orchestrator")
    bool SaveKnowledgeBase(const FString& FilePath);

    UFUNCTION(BlueprintCallable, Category = "IGED RAG Orchestrator")
    int32 GetDocumentCount() const;

    UFUNCTION(BlueprintCallable, Category = "IGED RAG Orchestrator")
    void ClearKnowledgeBase();

    UFUNCTION(BlueprintCallable, Category = "IGED RAG Orchestrator")
    TArray<FString> GetAvailableModels() const;

    UFUNCTION(BlueprintCallable, Category = "IGED RAG Orchestrator")
    bool SetActiveModel(const FString& ModelName);

    UPROPERTY(BlueprintAssignable, Category = "IGED RAG Orchestrator")
    FOnRAGQueryCompleted OnRAGQueryCompleted;

    UPROPERTY(BlueprintAssignable, Category = "IGED RAG Orchestrator")
    FOnRAGStatusChanged OnRAGStatusChanged;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    UIGEDModelManager* ModelManager;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, FIGEDVectorDocument> KnowledgeBase;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, FIGEDRAGQuery> ActiveQueries;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, FIGEDRAGResult> QueryResults;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    FString ActiveModelName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    int32 MaxRetrievalResults;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    float DefaultSimilarityThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    int32 EmbeddingDimensions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    FString EmbeddingModelEndpoint;

private:
    TMap<FString, ERAGStatus> QueryStatuses;
    FCriticalSection KnowledgeBaseMutex;
    FCriticalSection QueryMutex;

    void ProcessRAGPipeline(const FString& QueryID);
    TArray<FIGEDVectorDocument> RetrieveRelevantDocuments(const FString& Query, int32 MaxResults, float SimilarityThreshold);
    FString GenerateContextualResponse(const FString& Query, const TArray<FIGEDVectorDocument>& RetrievedDocs, ERAGTaskType TaskType);
    void UpdateQueryStatus(const FString& QueryID, ERAGStatus NewStatus);
    bool ValidateQuery(const FIGEDRAGQuery& Query);
    void InitializeDefaultKnowledgeBase();
    FString FormatContextForModel(const TArray<FIGEDVectorDocument>& Documents, const FString& Query, ERAGTaskType TaskType);
    void HandleModelResponse(const FString& ModelName, const FString& Response, bool bSuccess);
    void OptimizeKnowledgeBase();
    void CleanupCompletedQueries();
};

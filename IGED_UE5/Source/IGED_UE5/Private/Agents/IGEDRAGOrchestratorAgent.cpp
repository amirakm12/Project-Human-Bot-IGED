#include "Agents/IGEDRAGOrchestratorAgent.h"
#include "Engine/Engine.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Math/UnrealMathUtility.h"

UIGEDRAGOrchestratorAgent::UIGEDRAGOrchestratorAgent()
{
    ModelManager = nullptr;
    MaxRetrievalResults = 10;
    DefaultSimilarityThreshold = 0.7f;
    EmbeddingDimensions = 1536; // OpenAI embedding dimensions
    EmbeddingModelEndpoint = TEXT("https://api.openai.com/v1/embeddings");
}

bool UIGEDRAGOrchestratorAgent::ExecuteTask(const FString& TaskDescription, const TMap<FString, FString>& Parameters)
{
    UE_LOG(LogTemp, Warning, TEXT("IGED RAG Orchestrator: Executing task - %s"), *TaskDescription);
    
    // Create a RAG query from the task description
    FIGEDRAGQuery Query;
    Query.UserQuery = TaskDescription;
    Query.TaskType = ERAGTaskType::QuestionAnswering;
    Query.MaxResults = MaxRetrievalResults;
    Query.SimilarityThreshold = DefaultSimilarityThreshold;
    
    // Add parameters to the query
    for (const auto& ParamPair : Parameters)
    {
        Query.Parameters.Add(ParamPair.Key, ParamPair.Value);
    }
    
    // Process the query
    FString Result = ProcessQuery(Query);
    
    UE_LOG(LogTemp, Warning, TEXT("IGED RAG Orchestrator: Task completed with result: %s"), *Result);
    return !Result.IsEmpty();
}

FString UIGEDRAGOrchestratorAgent::GetAgentName() const
{
    return TEXT("RAG Orchestrator Agent");
}

FString UIGEDRAGOrchestratorAgent::GetStatus() const
{
    FString Status = FString::Printf(TEXT("Active - %d documents, %d active queries"), 
        KnowledgeBase.Num(), ActiveQueries.Num());
    return Status;
}

bool UIGEDRAGOrchestratorAgent::Initialize(UIGEDModelManager* InModelManager)
{
    if (!InModelManager)
    {
        UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Model manager is null"));
        return false;
    }
    
    ModelManager = InModelManager;
    
    // Initialize default knowledge base
    InitializeDefaultKnowledgeBase();
    
    // Set up model response handler
    ModelManager->OnModelResponse.AddDynamic(this, &UIGEDRAGOrchestratorAgent::HandleModelResponse);
    
    UE_LOG(LogTemp, Warning, TEXT("IGED RAG Orchestrator: Initialized successfully"));
    return true;
}

void UIGEDRAGOrchestratorAgent::Shutdown()
{
    UE_LOG(LogTemp, Warning, TEXT("IGED RAG Orchestrator: Shutting down..."));
    
    // Clear all data
    KnowledgeBase.Empty();
    ActiveQueries.Empty();
    QueryResults.Empty();
    QueryStatuses.Empty();
    
    ModelManager = nullptr;
}

FString UIGEDRAGOrchestratorAgent::ProcessQuery(const FIGEDRAGQuery& Query)
{
    if (!ValidateQuery(Query))
    {
        UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Invalid query"));
        return TEXT("");
    }
    
    // Store the query
    FScopeLock Lock(&QueryMutex);
    ActiveQueries.Add(Query.QueryID, Query);
    QueryStatuses.Add(Query.QueryID, ERAGStatus::Retrieving);
    
    // Process the RAG pipeline
    ProcessRAGPipeline(Query.QueryID);
    
    // Wait for completion (in a real implementation, this would be async)
    FTimerHandle TimerHandle;
    GEngine->GetWorldFromContextObject(this)->GetTimerManager().SetTimer(TimerHandle, [this, Query]()
    {
        if (const FIGEDRAGResult* Result = QueryResults.Find(Query.QueryID))
        {
            OnRAGQueryCompleted.Broadcast(Query.QueryID, *Result);
        }
    }, 5.0f, false);
    
    // Return the result if available
    if (const FIGEDRAGResult* Result = QueryResults.Find(Query.QueryID))
    {
        return Result->GeneratedResponse;
    }
    
    return TEXT("Processing...");
}

bool UIGEDRAGOrchestratorAgent::ProcessQueryAsync(const FIGEDRAGQuery& Query)
{
    if (!ValidateQuery(Query))
    {
        UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Invalid query"));
        return false;
    }
    
    // Store the query
    FScopeLock Lock(&QueryMutex);
    ActiveQueries.Add(Query.QueryID, Query);
    QueryStatuses.Add(Query.QueryID, ERAGStatus::Retrieving);
    
    // Process asynchronously
    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, Query]()
    {
        ProcessRAGPipeline(Query.QueryID);
    });
    
    return true;
}

bool UIGEDRAGOrchestratorAgent::AddDocument(const FIGEDVectorDocument& Document)
{
    if (Document.DocumentID.IsEmpty() || Document.Content.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Invalid document"));
        return false;
    }
    
    FScopeLock Lock(&KnowledgeBaseMutex);
    KnowledgeBase.Add(Document.DocumentID, Document);
    
    UE_LOG(LogTemp, Warning, TEXT("IGED RAG Orchestrator: Added document %s"), *Document.DocumentID);
    return true;
}

bool UIGEDRAGOrchestratorAgent::RemoveDocument(const FString& DocumentID)
{
    FScopeLock Lock(&KnowledgeBaseMutex);
    if (KnowledgeBase.Remove(DocumentID) > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("IGED RAG Orchestrator: Removed document %s"), *DocumentID);
        return true;
    }
    
    UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Document %s not found"), *DocumentID);
    return false;
}

bool UIGEDRAGOrchestratorAgent::UpdateDocument(const FString& DocumentID, const FIGEDVectorDocument& UpdatedDocument)
{
    FScopeLock Lock(&KnowledgeBaseMutex);
    if (KnowledgeBase.Contains(DocumentID))
    {
        KnowledgeBase[DocumentID] = UpdatedDocument;
        UE_LOG(LogTemp, Warning, TEXT("IGED RAG Orchestrator: Updated document %s"), *DocumentID);
        return true;
    }
    
    UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Document %s not found for update"), *DocumentID);
    return false;
}

TArray<FIGEDVectorDocument> UIGEDRAGOrchestratorAgent::SearchDocuments(const FString& Query, int32 MaxResults, float SimilarityThreshold)
{
    TArray<FIGEDVectorDocument> Results;
    
    if (Query.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Empty search query"));
        return Results;
    }
    
    // Generate embedding for the query
    TArray<float> QueryEmbedding;
    if (!GenerateEmbedding(Query, QueryEmbedding))
    {
        UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Failed to generate embedding for query"));
        return Results;
    }
    
    // Calculate similarities
    TArray<TPair<float, FIGEDVectorDocument>> ScoredDocuments;
    
    FScopeLock Lock(&KnowledgeBaseMutex);
    for (const auto& DocPair : KnowledgeBase)
    {
        float Similarity = CalculateSimilarity(QueryEmbedding, DocPair.Value.VectorEmbedding);
        if (Similarity >= SimilarityThreshold)
        {
            ScoredDocuments.Add(TPair<float, FIGEDVectorDocument>(Similarity, DocPair.Value));
        }
    }
    
    // Sort by similarity score (descending)
    ScoredDocuments.Sort([](const TPair<float, FIGEDVectorDocument>& A, const TPair<float, FIGEDVectorDocument>& B)
    {
        return A.Key > B.Key;
    });
    
    // Return top results
    int32 ResultCount = FMath::Min(MaxResults, ScoredDocuments.Num());
    for (int32 i = 0; i < ResultCount; i++)
    {
        Results.Add(ScoredDocuments[i].Value);
    }
    
    UE_LOG(LogTemp, Warning, TEXT("IGED RAG Orchestrator: Found %d relevant documents"), Results.Num());
    return Results;
}

bool UIGEDRAGOrchestratorAgent::GenerateEmbedding(const FString& Text, TArray<float>& OutEmbedding)
{
    if (Text.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Empty text for embedding"));
        return false;
    }
    
    // In a real implementation, this would call an embedding API
    // For now, we'll generate a mock embedding
    OutEmbedding.SetNum(EmbeddingDimensions);
    for (int32 i = 0; i < EmbeddingDimensions; i++)
    {
        OutEmbedding[i] = FMath::FRandRange(-1.0f, 1.0f);
    }
    
    // Normalize the embedding
    float Magnitude = 0.0f;
    for (float Value : OutEmbedding)
    {
        Magnitude += Value * Value;
    }
    Magnitude = FMath::Sqrt(Magnitude);
    
    if (Magnitude > 0.0f)
    {
        for (float& Value : OutEmbedding)
        {
            Value /= Magnitude;
        }
    }
    
    return true;
}

float UIGEDRAGOrchestratorAgent::CalculateSimilarity(const TArray<float>& Embedding1, const TArray<float>& Embedding2)
{
    if (Embedding1.Num() != Embedding2.Num() || Embedding1.Num() == 0)
    {
        return 0.0f;
    }
    
    // Calculate cosine similarity
    float DotProduct = 0.0f;
    float Magnitude1 = 0.0f;
    float Magnitude2 = 0.0f;
    
    for (int32 i = 0; i < Embedding1.Num(); i++)
    {
        DotProduct += Embedding1[i] * Embedding2[i];
        Magnitude1 += Embedding1[i] * Embedding1[i];
        Magnitude2 += Embedding2[i] * Embedding2[i];
    }
    
    Magnitude1 = FMath::Sqrt(Magnitude1);
    Magnitude2 = FMath::Sqrt(Magnitude2);
    
    if (Magnitude1 == 0.0f || Magnitude2 == 0.0f)
    {
        return 0.0f;
    }
    
    return DotProduct / (Magnitude1 * Magnitude2);
}

bool UIGEDRAGOrchestratorAgent::LoadKnowledgeBase(const FString& FilePath)
{
    if (!FPaths::FileExists(FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Knowledge base file not found: %s"), *FilePath);
        return false;
    }
    
    FString FileContent;
    if (!FFileHelper::LoadFileToString(FileContent, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Failed to load knowledge base file"));
        return false;
    }
    
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContent);
    
    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Failed to parse knowledge base JSON"));
        return false;
    }
    
    FScopeLock Lock(&KnowledgeBaseMutex);
    KnowledgeBase.Empty();
    
    const TArray<TSharedPtr<FJsonValue>>* DocumentsArray;
    if (JsonObject->TryGetArrayField(TEXT("documents"), DocumentsArray))
    {
        for (const TSharedPtr<FJsonValue>& DocumentValue : *DocumentsArray)
        {
            TSharedPtr<FJsonObject> DocumentObj = DocumentValue->AsObject();
            if (DocumentObj.IsValid())
            {
                FIGEDVectorDocument Document;
                DocumentObj->TryGetStringField(TEXT("documentID"), Document.DocumentID);
                DocumentObj->TryGetStringField(TEXT("title"), Document.Title);
                DocumentObj->TryGetStringField(TEXT("content"), Document.Content);
                DocumentObj->TryGetStringField(TEXT("source"), Document.Source);
                
                // Parse embedding
                const TArray<TSharedPtr<FJsonValue>>* EmbeddingArray;
                if (DocumentObj->TryGetArrayField(TEXT("vectorEmbedding"), EmbeddingArray))
                {
                    Document.VectorEmbedding.SetNum(EmbeddingArray->Num());
                    for (int32 i = 0; i < EmbeddingArray->Num(); i++)
                    {
                        Document.VectorEmbedding[i] = (*EmbeddingArray)[i]->AsNumber();
                    }
                }
                
                if (!Document.DocumentID.IsEmpty())
                {
                    KnowledgeBase.Add(Document.DocumentID, Document);
                }
            }
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("IGED RAG Orchestrator: Loaded %d documents from knowledge base"), KnowledgeBase.Num());
    return true;
}

bool UIGEDRAGOrchestratorAgent::SaveKnowledgeBase(const FString& FilePath)
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    TArray<TSharedPtr<FJsonValue>> DocumentsArray;
    
    FScopeLock Lock(&KnowledgeBaseMutex);
    for (const auto& DocPair : KnowledgeBase)
    {
        TSharedPtr<FJsonObject> DocumentObj = MakeShareable(new FJsonObject);
        DocumentObj->SetStringField(TEXT("documentID"), DocPair.Value.DocumentID);
        DocumentObj->SetStringField(TEXT("title"), DocPair.Value.Title);
        DocumentObj->SetStringField(TEXT("content"), DocPair.Value.Content);
        DocumentObj->SetStringField(TEXT("source"), DocPair.Value.Source);
        
        // Serialize embedding
        TArray<TSharedPtr<FJsonValue>> EmbeddingArray;
        for (float Value : DocPair.Value.VectorEmbedding)
        {
            EmbeddingArray.Add(MakeShareable(new FJsonValueNumber(Value)));
        }
        DocumentObj->SetArrayField(TEXT("vectorEmbedding"), EmbeddingArray);
        
        DocumentsArray.Add(MakeShareable(new FJsonValueObject(DocumentObj)));
    }
    
    JsonObject->SetArrayField(TEXT("documents"), DocumentsArray);
    
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    
    if (!FFileHelper::SaveStringToFile(OutputString, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Failed to save knowledge base"));
        return false;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("IGED RAG Orchestrator: Saved %d documents to knowledge base"), KnowledgeBase.Num());
    return true;
}

int32 UIGEDRAGOrchestratorAgent::GetDocumentCount() const
{
    FScopeLock Lock(&KnowledgeBaseMutex);
    return KnowledgeBase.Num();
}

void UIGEDRAGOrchestratorAgent::ClearKnowledgeBase()
{
    FScopeLock Lock(&KnowledgeBaseMutex);
    KnowledgeBase.Empty();
    UE_LOG(LogTemp, Warning, TEXT("IGED RAG Orchestrator: Cleared knowledge base"));
}

TArray<FString> UIGEDRAGOrchestratorAgent::GetAvailableModels() const
{
    if (ModelManager)
    {
        return ModelManager->GetAvailableModels();
    }
    return TArray<FString>();
}

bool UIGEDRAGOrchestratorAgent::SetActiveModel(const FString& ModelName)
{
    if (!ModelManager)
    {
        UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Model manager not initialized"));
        return false;
    }
    
    if (ModelManager->SwitchActiveModel(ModelName))
    {
        ActiveModelName = ModelName;
        UE_LOG(LogTemp, Warning, TEXT("IGED RAG Orchestrator: Switched to model %s"), *ModelName);
        return true;
    }
    
    return false;
}

void UIGEDRAGOrchestratorAgent::ProcessRAGPipeline(const FString& QueryID)
{
    FScopeLock Lock(&QueryMutex);
    if (!ActiveQueries.Contains(QueryID))
    {
        UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Query %s not found"), *QueryID);
        return;
    }
    
    const FIGEDRAGQuery& Query = ActiveQueries[QueryID];
    UpdateQueryStatus(QueryID, ERAGStatus::Retrieving);
    
    // Step 1: Retrieve relevant documents
    TArray<FIGEDVectorDocument> RetrievedDocs = RetrieveRelevantDocuments(
        Query.UserQuery, Query.MaxResults, Query.SimilarityThreshold);
    
    UpdateQueryStatus(QueryID, ERAGStatus::Processing);
    
    // Step 2: Generate contextual response
    FString GeneratedResponse = GenerateContextualResponse(
        Query.UserQuery, RetrievedDocs, Query.TaskType);
    
    UpdateQueryStatus(QueryID, ERAGStatus::Completed);
    
    // Step 3: Create result
    FIGEDRAGResult Result;
    Result.QueryID = QueryID;
    Result.GeneratedResponse = GeneratedResponse;
    Result.Confidence = 0.85f; // Mock confidence
    Result.Status = ERAGStatus::Completed;
    Result.ProcessingTime = FDateTime::Now();
    
    // Add retrieved documents to result
    for (const FIGEDVectorDocument& Doc : RetrievedDocs)
    {
        Result.RetrievedDocuments.Add(Doc.DocumentID);
        Result.Sources.Add(Doc.Source);
    }
    
    QueryResults.Add(QueryID, Result);
    
    UE_LOG(LogTemp, Warning, TEXT("IGED RAG Orchestrator: Completed query %s"), *QueryID);
}

TArray<FIGEDVectorDocument> UIGEDRAGOrchestratorAgent::RetrieveRelevantDocuments(const FString& Query, int32 MaxResults, float SimilarityThreshold)
{
    return SearchDocuments(Query, MaxResults, SimilarityThreshold);
}

FString UIGEDRAGOrchestratorAgent::GenerateContextualResponse(const FString& Query, const TArray<FIGEDVectorDocument>& RetrievedDocs, ERAGTaskType TaskType)
{
    if (!ModelManager)
    {
        UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Model manager not available"));
        return TEXT("Error: Model manager not available");
    }
    
    // Format context for the model
    FString ContextualPrompt = FormatContextForModel(RetrievedDocs, Query, TaskType);
    
    // Send request to the active model
    TMap<FString, FString> Parameters;
    Parameters.Add(TEXT("task_type"), FString::FromInt((int32)TaskType));
    
    if (ModelManager->SendAsyncRequest(ActiveModelName, ContextualPrompt, Parameters))
    {
        return TEXT("Response generated successfully");
    }
    
    return TEXT("Error: Failed to generate response");
}

void UIGEDRAGOrchestratorAgent::UpdateQueryStatus(const FString& QueryID, ERAGStatus NewStatus)
{
    FScopeLock Lock(&QueryMutex);
    ERAGStatus OldStatus = ERAGStatus::Idle;
    if (ERAGStatus* Status = QueryStatuses.Find(QueryID))
    {
        OldStatus = *Status;
        *Status = NewStatus;
    }
    else
    {
        QueryStatuses.Add(QueryID, NewStatus);
    }
    
    OnRAGStatusChanged.Broadcast(QueryID, OldStatus, NewStatus);
}

bool UIGEDRAGOrchestratorAgent::ValidateQuery(const FIGEDRAGQuery& Query)
{
    if (Query.QueryID.IsEmpty() || Query.UserQuery.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Invalid query - missing ID or content"));
        return false;
    }
    
    if (Query.MaxResults <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Invalid query - max results must be positive"));
        return false;
    }
    
    if (Query.SimilarityThreshold < 0.0f || Query.SimilarityThreshold > 1.0f)
    {
        UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Invalid query - similarity threshold must be between 0 and 1"));
        return false;
    }
    
    return true;
}

void UIGEDRAGOrchestratorAgent::InitializeDefaultKnowledgeBase()
{
    // Add some default knowledge documents
    FIGEDVectorDocument Doc1;
    Doc1.DocumentID = TEXT("doc_001");
    Doc1.Title = TEXT("IGED System Overview");
    Doc1.Content = TEXT("IGED is an advanced AI assistant system that combines multiple AI models and agents to provide comprehensive assistance. It includes voice recognition, natural language processing, and various specialized agents for different tasks.");
    Doc1.Source = TEXT("system_docs");
    GenerateEmbedding(Doc1.Content, Doc1.VectorEmbedding);
    KnowledgeBase.Add(Doc1.DocumentID, Doc1);
    
    FIGEDVectorDocument Doc2;
    Doc2.DocumentID = TEXT("doc_002");
    Doc2.Title = TEXT("RAG System Architecture");
    Doc2.Content = TEXT("Retrieval-Augmented Generation (RAG) combines information retrieval with text generation. It retrieves relevant documents from a knowledge base and uses them as context to generate more accurate and informed responses.");
    Doc2.Source = TEXT("technical_docs");
    GenerateEmbedding(Doc2.Content, Doc2.VectorEmbedding);
    KnowledgeBase.Add(Doc2.DocumentID, Doc2);
    
    FIGEDVectorDocument Doc3;
    Doc3.DocumentID = TEXT("doc_003");
    Doc3.Title = TEXT("AI Model Integration");
    Doc3.Content = TEXT("The system supports multiple AI models including GPT-4, Claude 3, Gemini, Llama, and others. Models can be dynamically loaded, switched, and optimized based on performance metrics and task requirements.");
    Doc3.Source = TEXT("model_docs");
    GenerateEmbedding(Doc3.Content, Doc3.VectorEmbedding);
    KnowledgeBase.Add(Doc3.DocumentID, Doc3);
    
    UE_LOG(LogTemp, Warning, TEXT("IGED RAG Orchestrator: Initialized with %d default documents"), KnowledgeBase.Num());
}

FString UIGEDRAGOrchestratorAgent::FormatContextForModel(const TArray<FIGEDVectorDocument>& Documents, const FString& Query, ERAGTaskType TaskType)
{
    FString ContextualPrompt = TEXT("Based on the following context documents, please answer the user's question:\n\n");
    
    // Add context documents
    for (int32 i = 0; i < Documents.Num(); i++)
    {
        ContextualPrompt += FString::Printf(TEXT("Document %d: %s\n%s\n\n"), 
            i + 1, *Documents[i].Title, *Documents[i].Content);
    }
    
    // Add the user's question
    ContextualPrompt += FString::Printf(TEXT("User Question: %s\n\n"), *Query);
    
    // Add task-specific instructions
    switch (TaskType)
    {
        case ERAGTaskType::QuestionAnswering:
            ContextualPrompt += TEXT("Please provide a comprehensive answer based on the context above.");
            break;
        case ERAGTaskType::CodeGeneration:
            ContextualPrompt += TEXT("Please generate code based on the context and requirements above.");
            break;
        case ERAGTaskType::DocumentAnalysis:
            ContextualPrompt += TEXT("Please analyze the documents and provide insights.");
            break;
        case ERAGTaskType::ResearchSynthesis:
            ContextualPrompt += TEXT("Please synthesize the information and provide a research summary.");
            break;
        default:
            ContextualPrompt += TEXT("Please provide a helpful response based on the context above.");
            break;
    }
    
    return ContextualPrompt;
}

void UIGEDRAGOrchestratorAgent::HandleModelResponse(const FString& ModelName, const FString& Response, bool bSuccess)
{
    if (bSuccess)
    {
        UE_LOG(LogTemp, Warning, TEXT("IGED RAG Orchestrator: Received response from model %s"), *ModelName);
        // Process the response and update query results
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("IGED RAG Orchestrator: Failed to get response from model %s"), *ModelName);
    }
}

void UIGEDRAGOrchestratorAgent::OptimizeKnowledgeBase()
{
    // Implement knowledge base optimization logic
    UE_LOG(LogTemp, Warning, TEXT("IGED RAG Orchestrator: Optimizing knowledge base..."));
}

void UIGEDRAGOrchestratorAgent::CleanupCompletedQueries()
{
    FScopeLock Lock(&QueryMutex);
    
    TArray<FString> QueriesToRemove;
    for (const auto& StatusPair : QueryStatuses)
    {
        if (StatusPair.Value == ERAGStatus::Completed || StatusPair.Value == ERAGStatus::Error)
        {
            QueriesToRemove.Add(StatusPair.Key);
        }
    }
    
    for (const FString& QueryID : QueriesToRemove)
    {
        ActiveQueries.Remove(QueryID);
        QueryStatuses.Remove(QueryID);
        QueryResults.Remove(QueryID);
    }
    
    if (QueriesToRemove.Num() > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("IGED RAG Orchestrator: Cleaned up %d completed queries"), QueriesToRemove.Num());
    }
}

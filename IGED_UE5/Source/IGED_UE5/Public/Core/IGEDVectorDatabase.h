// IGED Vector Database - Advanced Vector Storage and Retrieval
// Copyright (c) 2024 Project Human Bot

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Dom/JsonObject.h"
#include "IGEDVectorDatabase.generated.h"

// Vector embedding structure
USTRUCT(BlueprintType)
struct FVectorEmbedding
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "IGED|VectorDB")
	FString DocumentID;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|VectorDB")
	FString Content;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|VectorDB")
	TArray<float> Embedding;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|VectorDB")
	TMap<FString, FString> Metadata;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|VectorDB")
	FDateTime CreatedAt;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|VectorDB")
	FDateTime UpdatedAt;

	FVectorEmbedding()
		: CreatedAt(FDateTime::Now())
		, UpdatedAt(FDateTime::Now())
	{}
};

// Search result structure
USTRUCT(BlueprintType)
struct FVectorSearchResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "IGED|VectorDB")
	FString DocumentID;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|VectorDB")
	FString Content;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|VectorDB")
	float SimilarityScore;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|VectorDB")
	TMap<FString, FString> Metadata;

	FVectorSearchResult()
		: SimilarityScore(0.0f)
	{}
};

// Database configuration
USTRUCT(BlueprintType)
struct FVectorDatabaseConfig
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "IGED|VectorDB")
	FString DatabaseName;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|VectorDB")
	FString StoragePath;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|VectorDB")
	int32 EmbeddingDimensions;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|VectorDB")
	FString EmbeddingModel;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|VectorDB")
	int32 MaxResults;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|VectorDB")
	float SimilarityThreshold;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|VectorDB")
	bool bUseIndexing;

	UPROPERTY(BlueprintReadWrite, Category = "IGED|VectorDB")
	int32 IndexUpdateInterval;

	FVectorDatabaseConfig()
		: EmbeddingDimensions(768)
		, MaxResults(10)
		, SimilarityThreshold(0.7f)
		, bUseIndexing(true)
		, IndexUpdateInterval(100)
	{}
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnVectorSearchComplete, const TArray<FVectorSearchResult>&, Results, bool, bSuccess);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnVectorInsertComplete, const FString&, DocumentID, bool, bSuccess);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnVectorDatabaseReady, bool, bSuccess);

UCLASS(Blueprintable)
class IGED_UE5_API UIGEDVectorDatabase : public UObject
{
	GENERATED_BODY()

public:
	UIGEDVectorDatabase();

	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void Initialize(const FVectorDatabaseConfig& Config);

	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void Shutdown();

	// Vector operations
	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void InsertVector(const FVectorEmbedding& Embedding, const FOnVectorInsertComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void InsertVectors(const TArray<FVectorEmbedding>& Embeddings, const FOnVectorInsertComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void SearchSimilarVectors(const TArray<float>& QueryEmbedding, int32 MaxResults, const FOnVectorSearchComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void SearchByText(const FString& QueryText, int32 MaxResults, const FOnVectorSearchComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void UpdateVector(const FString& DocumentID, const FVectorEmbedding& NewEmbedding, const FOnVectorInsertComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void DeleteVector(const FString& DocumentID, const FOnVectorInsertComplete& OnComplete);

	// Batch operations
	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void BatchInsertFromDocuments(const TArray<FString>& DocumentPaths, const FOnVectorInsertComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void BatchUpdateFromDocuments(const TArray<FString>& DocumentPaths, const FOnVectorInsertComplete& OnComplete);

	// Database management
	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void RebuildIndex();

	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void OptimizeDatabase();

	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void BackupDatabase(const FString& BackupPath);

	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void RestoreDatabase(const FString& BackupPath);

	// Statistics and monitoring
	UFUNCTION(BlueprintPure, Category = "IGED|VectorDB")
	int32 GetVectorCount() const;

	UFUNCTION(BlueprintPure, Category = "IGED|VectorDB")
	FString GetDatabaseInfo() const;

	UFUNCTION(BlueprintPure, Category = "IGED|VectorDB")
	bool IsInitialized() const;

	UFUNCTION(BlueprintPure, Category = "IGED|VectorDB")
	FVectorDatabaseConfig GetConfig() const;

	// Advanced search capabilities
	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void SearchWithFilters(const TArray<float>& QueryEmbedding, const TMap<FString, FString>& MetadataFilters, int32 MaxResults, const FOnVectorSearchComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void SearchByMetadata(const TMap<FString, FString>& MetadataFilters, int32 MaxResults, const FOnVectorSearchComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "IGED|VectorDB")
	void HybridSearch(const FString& TextQuery, const TArray<float>& VectorQuery, float TextWeight, float VectorWeight, int32 MaxResults, const FOnVectorSearchComplete& OnComplete);

protected:
	UPROPERTY()
	bool bIsInitialized;

	UPROPERTY()
	FVectorDatabaseConfig DatabaseConfig;

	UPROPERTY()
	TMap<FString, FVectorEmbedding> VectorStorage;

	UPROPERTY()
	int32 VectorCount;

	UPROPERTY()
	FDateTime LastIndexUpdate;

private:
	// Internal methods
	TArray<float> GenerateEmbedding(const FString& Text);
	float CalculateCosineSimilarity(const TArray<float>& VectorA, const TArray<float>& VectorB);
	TArray<FVectorSearchResult> PerformSimilaritySearch(const TArray<float>& QueryEmbedding, int32 MaxResults);
	void UpdateIndex();
	void SaveDatabaseToDisk();
	void LoadDatabaseFromDisk();
	FString GenerateDocumentID(const FString& Content);
	void LogVectorDBEvent(const FString& Event, const FString& Details);
};

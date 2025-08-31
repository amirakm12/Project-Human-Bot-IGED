// IGED Vector Database - Advanced Vector Storage and Retrieval Implementation
// Copyright (c) 2024 Project Human Bot

#include "Core/IGEDVectorDatabase.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Serialization/JsonSerializer.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Engine/Engine.h"

UIGEDVectorDatabase::UIGEDVectorDatabase()
	: bIsInitialized(false)
	, VectorCount(0)
{
	// Initialize vector database
}

void UIGEDVectorDatabase::Initialize(const FVectorDatabaseConfig& Config)
{
	if (bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("IGEDVectorDatabase already initialized."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("IGEDVectorDatabase initializing with config: %s"), *Config.DatabaseName);
	
	DatabaseConfig = Config;
	
	// Create storage directory if it doesn't exist
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*Config.StoragePath))
	{
		PlatformFile.CreateDirectoryTree(*Config.StoragePath);
	}
	
	// Load existing database from disk
	LoadDatabaseFromDisk();
	
	bIsInitialized = true;
	UE_LOG(LogTemp, Log, TEXT("IGEDVectorDatabase initialized. Loaded %d vectors."), VectorCount);
}

void UIGEDVectorDatabase::Shutdown()
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("IGEDVectorDatabase not initialized. Nothing to shut down."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("IGEDVectorDatabase shutting down..."));
	
	// Save database to disk
	SaveDatabaseToDisk();
	
	bIsInitialized = false;
	UE_LOG(LogTemp, Log, TEXT("IGEDVectorDatabase shut down."));
}

void UIGEDVectorDatabase::InsertVector(const FVectorEmbedding& Embedding, const FOnVectorInsertComplete& OnComplete)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Error, TEXT("IGEDVectorDatabase not initialized. Cannot insert vector."));
		OnComplete.ExecuteIfBound(TEXT(""), false);
		return;
	}

	FString DocumentID = Embedding.DocumentID.IsEmpty() ? GenerateDocumentID(Embedding.Content) : Embedding.DocumentID;
	
	// Create a copy with the generated ID
	FVectorEmbedding NewEmbedding = Embedding;
	NewEmbedding.DocumentID = DocumentID;
	NewEmbedding.UpdatedAt = FDateTime::Now();
	
	// Store the vector
	VectorStorage.Add(DocumentID, NewEmbedding);
	VectorCount++;
	
	// Update index if needed
	if (DatabaseConfig.bUseIndexing && VectorCount % DatabaseConfig.IndexUpdateInterval == 0)
	{
		UpdateIndex();
	}
	
	UE_LOG(LogTemp, Log, TEXT("Vector inserted with ID: %s"), *DocumentID);
	OnComplete.ExecuteIfBound(DocumentID, true);
	LogVectorDBEvent(TEXT("VectorInserted"), FString::Printf(TEXT("ID: %s, Dimensions: %d"), *DocumentID, NewEmbedding.Embedding.Num()));
}

void UIGEDVectorDatabase::InsertVectors(const TArray<FVectorEmbedding>& Embeddings, const FOnVectorInsertComplete& OnComplete)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Error, TEXT("IGEDVectorDatabase not initialized. Cannot insert vectors."));
		OnComplete.ExecuteIfBound(TEXT(""), false);
		return;
	}

	int32 InsertedCount = 0;
	for (const FVectorEmbedding& Embedding : Embeddings)
	{
		FString DocumentID = Embedding.DocumentID.IsEmpty() ? GenerateDocumentID(Embedding.Content) : Embedding.DocumentID;
		
		FVectorEmbedding NewEmbedding = Embedding;
		NewEmbedding.DocumentID = DocumentID;
		NewEmbedding.UpdatedAt = FDateTime::Now();
		
		VectorStorage.Add(DocumentID, NewEmbedding);
		InsertedCount++;
	}
	
	VectorCount += InsertedCount;
	
	// Update index after batch insert
	if (DatabaseConfig.bUseIndexing)
	{
		UpdateIndex();
	}
	
	UE_LOG(LogTemp, Log, TEXT("Batch inserted %d vectors"), InsertedCount);
	OnComplete.ExecuteIfBound(FString::Printf(TEXT("Batch_%d_vectors"), InsertedCount), true);
	LogVectorDBEvent(TEXT("BatchVectorInserted"), FString::Printf(TEXT("Count: %d"), InsertedCount));
}

void UIGEDVectorDatabase::SearchSimilarVectors(const TArray<float>& QueryEmbedding, int32 MaxResults, const FOnVectorSearchComplete& OnComplete)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Error, TEXT("IGEDVectorDatabase not initialized. Cannot search vectors."));
		TArray<FVectorSearchResult> EmptyResults;
		OnComplete.ExecuteIfBound(EmptyResults, false);
		return;
	}

	if (QueryEmbedding.Num() != DatabaseConfig.EmbeddingDimensions)
	{
		UE_LOG(LogTemp, Error, TEXT("Query embedding dimensions (%d) don't match database dimensions (%d)"), QueryEmbedding.Num(), DatabaseConfig.EmbeddingDimensions);
		TArray<FVectorSearchResult> EmptyResults;
		OnComplete.ExecuteIfBound(EmptyResults, false);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Searching for similar vectors with %d dimensions"), QueryEmbedding.Num());
	
	TArray<FVectorSearchResult> Results = PerformSimilaritySearch(QueryEmbedding, MaxResults);
	
	OnComplete.ExecuteIfBound(Results, true);
	LogVectorDBEvent(TEXT("VectorSearch"), FString::Printf(TEXT("Query dimensions: %d, Results: %d"), QueryEmbedding.Num(), Results.Num()));
}

void UIGEDVectorDatabase::SearchByText(const FString& QueryText, int32 MaxResults, const FOnVectorSearchComplete& OnComplete)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Error, TEXT("IGEDVectorDatabase not initialized. Cannot search by text."));
		TArray<FVectorSearchResult> EmptyResults;
		OnComplete.ExecuteIfBound(EmptyResults, false);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Searching by text: %s"), *QueryText.Left(100));
	
	// Generate embedding for the query text
	TArray<float> QueryEmbedding = GenerateEmbedding(QueryText);
	
	if (QueryEmbedding.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate embedding for query text"));
		TArray<FVectorSearchResult> EmptyResults;
		OnComplete.ExecuteIfBound(EmptyResults, false);
		return;
	}
	
	// Perform similarity search
	TArray<FVectorSearchResult> Results = PerformSimilaritySearch(QueryEmbedding, MaxResults);
	
	OnComplete.ExecuteIfBound(Results, true);
	LogVectorDBEvent(TEXT("TextSearch"), FString::Printf(TEXT("Query: %s, Results: %d"), *QueryText.Left(50), Results.Num()));
}

void UIGEDVectorDatabase::UpdateVector(const FString& DocumentID, const FVectorEmbedding& NewEmbedding, const FOnVectorInsertComplete& OnComplete)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Error, TEXT("IGEDVectorDatabase not initialized. Cannot update vector."));
		OnComplete.ExecuteIfBound(TEXT(""), false);
		return;
	}

	if (!VectorStorage.Contains(DocumentID))
	{
		UE_LOG(LogTemp, Error, TEXT("Vector with ID %s not found for update"), *DocumentID);
		OnComplete.ExecuteIfBound(TEXT(""), false);
		return;
	}

	FVectorEmbedding UpdatedEmbedding = NewEmbedding;
	UpdatedEmbedding.DocumentID = DocumentID;
	UpdatedEmbedding.UpdatedAt = FDateTime::Now();
	
	VectorStorage[DocumentID] = UpdatedEmbedding;
	
	UE_LOG(LogTemp, Log, TEXT("Vector updated with ID: %s"), *DocumentID);
	OnComplete.ExecuteIfBound(DocumentID, true);
	LogVectorDBEvent(TEXT("VectorUpdated"), FString::Printf(TEXT("ID: %s"), *DocumentID));
}

void UIGEDVectorDatabase::DeleteVector(const FString& DocumentID, const FOnVectorInsertComplete& OnComplete)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Error, TEXT("IGEDVectorDatabase not initialized. Cannot delete vector."));
		OnComplete.ExecuteIfBound(TEXT(""), false);
		return;
	}

	if (!VectorStorage.Contains(DocumentID))
	{
		UE_LOG(LogTemp, Error, TEXT("Vector with ID %s not found for deletion"), *DocumentID);
		OnComplete.ExecuteIfBound(TEXT(""), false);
		return;
	}

	VectorStorage.Remove(DocumentID);
	VectorCount--;
	
	UE_LOG(LogTemp, Log, TEXT("Vector deleted with ID: %s"), *DocumentID);
	OnComplete.ExecuteIfBound(DocumentID, true);
	LogVectorDBEvent(TEXT("VectorDeleted"), FString::Printf(TEXT("ID: %s"), *DocumentID));
}

void UIGEDVectorDatabase::BatchInsertFromDocuments(const TArray<FString>& DocumentPaths, const FOnVectorInsertComplete& OnComplete)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Error, TEXT("IGEDVectorDatabase not initialized. Cannot batch insert from documents."));
		OnComplete.ExecuteIfBound(TEXT(""), false);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Batch inserting from %d documents"), DocumentPaths.Num());
	
	int32 ProcessedCount = 0;
	TArray<FVectorEmbedding> Embeddings;
	
	for (const FString& DocumentPath : DocumentPaths)
	{
		// In a real implementation, this would:
		// 1. Read the document file
		// 2. Chunk the content
		// 3. Generate embeddings for each chunk
		// 4. Create FVectorEmbedding objects
		
		// For simulation, create a mock embedding
		FVectorEmbedding Embedding;
		Embedding.Content = FString::Printf(TEXT("Document content from: %s"), *DocumentPath);
		Embedding.Embedding = GenerateEmbedding(Embedding.Content);
		Embedding.Metadata.Add(TEXT("source_path"), DocumentPath);
		Embedding.Metadata.Add(TEXT("file_type"), FPaths::GetExtension(DocumentPath));
		
		Embeddings.Add(Embedding);
		ProcessedCount++;
	}
	
	// Insert all embeddings
	InsertVectors(Embeddings, OnComplete);
	
	LogVectorDBEvent(TEXT("BatchDocumentInsert"), FString::Printf(TEXT("Documents: %d, Processed: %d"), DocumentPaths.Num(), ProcessedCount));
}

void UIGEDVectorDatabase::BatchUpdateFromDocuments(const TArray<FString>& DocumentPaths, const FOnVectorInsertComplete& OnComplete)
{
	// Similar to batch insert but for updates
	UE_LOG(LogTemp, Log, TEXT("Batch updating from %d documents"), DocumentPaths.Num());
	
	// For now, treat as batch insert
	BatchInsertFromDocuments(DocumentPaths, OnComplete);
}

void UIGEDVectorDatabase::RebuildIndex()
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Error, TEXT("IGEDVectorDatabase not initialized. Cannot rebuild index."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Rebuilding vector database index..."));
	
	UpdateIndex();
	LastIndexUpdate = FDateTime::Now();
	
	UE_LOG(LogTemp, Log, TEXT("Index rebuild completed"));
	LogVectorDBEvent(TEXT("IndexRebuilt"), FString::Printf(TEXT("Vectors: %d"), VectorCount));
}

void UIGEDVectorDatabase::OptimizeDatabase()
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Error, TEXT("IGEDVectorDatabase not initialized. Cannot optimize database."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Optimizing vector database..."));
	
	// In a real implementation, this would:
	// 1. Remove duplicate vectors
	// 2. Optimize storage layout
	// 3. Compress data
	// 4. Update indices
	
	UE_LOG(LogTemp, Log, TEXT("Database optimization completed"));
	LogVectorDBEvent(TEXT("DatabaseOptimized"), FString::Printf(TEXT("Vectors: %d"), VectorCount));
}

void UIGEDVectorDatabase::BackupDatabase(const FString& BackupPath)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Error, TEXT("IGEDVectorDatabase not initialized. Cannot backup database."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Backing up database to: %s"), *BackupPath);
	
	// In a real implementation, this would create a backup file
	// For now, just save the current state
	SaveDatabaseToDisk();
	
	UE_LOG(LogTemp, Log, TEXT("Database backup completed"));
	LogVectorDBEvent(TEXT("DatabaseBackup"), FString::Printf(TEXT("Path: %s"), *BackupPath));
}

void UIGEDVectorDatabase::RestoreDatabase(const FString& BackupPath)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Error, TEXT("IGEDVectorDatabase not initialized. Cannot restore database."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Restoring database from: %s"), *BackupPath);
	
	// In a real implementation, this would restore from backup
	// For now, just reload from disk
	LoadDatabaseFromDisk();
	
	UE_LOG(LogTemp, Log, TEXT("Database restore completed"));
	LogVectorDBEvent(TEXT("DatabaseRestore"), FString::Printf(TEXT("Path: %s"), *BackupPath));
}

int32 UIGEDVectorDatabase::GetVectorCount() const
{
	return VectorCount;
}

FString UIGEDVectorDatabase::GetDatabaseInfo() const
{
	if (!bIsInitialized)
	{
		return TEXT("Database not initialized");
	}

	return FString::Printf(TEXT("Database: %s\nVectors: %d\nDimensions: %d\nStorage: %s\nLast Index Update: %s"),
		*DatabaseConfig.DatabaseName,
		VectorCount,
		DatabaseConfig.EmbeddingDimensions,
		*DatabaseConfig.StoragePath,
		*LastIndexUpdate.ToString()
	);
}

bool UIGEDVectorDatabase::IsInitialized() const
{
	return bIsInitialized;
}

FVectorDatabaseConfig UIGEDVectorDatabase::GetConfig() const
{
	return DatabaseConfig;
}

void UIGEDVectorDatabase::SearchWithFilters(const TArray<float>& QueryEmbedding, const TMap<FString, FString>& MetadataFilters, int32 MaxResults, const FOnVectorSearchComplete& OnComplete)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Error, TEXT("IGEDVectorDatabase not initialized. Cannot search with filters."));
		TArray<FVectorSearchResult> EmptyResults;
		OnComplete.ExecuteIfBound(EmptyResults, false);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Searching with %d metadata filters"), MetadataFilters.Num());
	
	TArray<FVectorSearchResult> Results = PerformSimilaritySearch(QueryEmbedding, MaxResults);
	
	// Apply metadata filters
	TArray<FVectorSearchResult> FilteredResults;
	for (const FVectorSearchResult& Result : Results)
	{
		bool bPassesFilters = true;
		for (const auto& Filter : MetadataFilters)
		{
			if (!Result.Metadata.Contains(Filter.Key) || Result.Metadata[Filter.Key] != Filter.Value)
			{
				bPassesFilters = false;
				break;
			}
		}
		
		if (bPassesFilters)
		{
			FilteredResults.Add(Result);
		}
	}
	
	OnComplete.ExecuteIfBound(FilteredResults, true);
	LogVectorDBEvent(TEXT("FilteredSearch"), FString::Printf(TEXT("Filters: %d, Results: %d"), MetadataFilters.Num(), FilteredResults.Num()));
}

void UIGEDVectorDatabase::SearchByMetadata(const TMap<FString, FString>& MetadataFilters, int32 MaxResults, const FOnVectorSearchComplete& OnComplete)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Error, TEXT("IGEDVectorDatabase not initialized. Cannot search by metadata."));
		TArray<FVectorSearchResult> EmptyResults;
		OnComplete.ExecuteIfBound(EmptyResults, false);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Searching by metadata with %d filters"), MetadataFilters.Num());
	
	TArray<FVectorSearchResult> Results;
	int32 ResultCount = 0;
	
	for (const auto& VectorPair : VectorStorage)
	{
		if (ResultCount >= MaxResults)
		{
			break;
		}
		
		const FVectorEmbedding& Embedding = VectorPair.Value;
		bool bPassesFilters = true;
		
		for (const auto& Filter : MetadataFilters)
		{
			if (!Embedding.Metadata.Contains(Filter.Key) || Embedding.Metadata[Filter.Key] != Filter.Value)
			{
				bPassesFilters = false;
				break;
			}
		}
		
		if (bPassesFilters)
		{
			FVectorSearchResult Result;
			Result.DocumentID = Embedding.DocumentID;
			Result.Content = Embedding.Content;
			Result.SimilarityScore = 1.0f; // Perfect match for metadata search
			Result.Metadata = Embedding.Metadata;
			Results.Add(Result);
			ResultCount++;
		}
	}
	
	OnComplete.ExecuteIfBound(Results, true);
	LogVectorDBEvent(TEXT("MetadataSearch"), FString::Printf(TEXT("Filters: %d, Results: %d"), MetadataFilters.Num(), Results.Num()));
}

void UIGEDVectorDatabase::HybridSearch(const FString& TextQuery, const TArray<float>& VectorQuery, float TextWeight, float VectorWeight, int32 MaxResults, const FOnVectorSearchComplete& OnComplete)
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Error, TEXT("IGEDVectorDatabase not initialized. Cannot perform hybrid search."));
		TArray<FVectorSearchResult> EmptyResults;
		OnComplete.ExecuteIfBound(EmptyResults, false);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Performing hybrid search with text weight: %.2f, vector weight: %.2f"), TextWeight, VectorWeight);
	
	// Perform both text and vector searches
	TArray<FVectorSearchResult> TextResults;
	TArray<FVectorSearchResult> VectorResults;
	
	// Text search
	SearchByText(TextQuery, MaxResults, FOnVectorSearchComplete::CreateLambda([this, VectorQuery, TextWeight, VectorWeight, MaxResults, OnComplete](const TArray<FVectorSearchResult>& Results, bool bSuccess)
	{
		if (bSuccess)
		{
			// Vector search
			SearchSimilarVectors(VectorQuery, MaxResults, FOnVectorSearchComplete::CreateLambda([TextWeight, VectorWeight, OnComplete](const TArray<FVectorSearchResult>& VectorResults, bool bVectorSuccess)
			{
				if (bVectorSuccess)
				{
					// Combine and rank results
					TArray<FVectorSearchResult> CombinedResults;
					// In a real implementation, this would combine and re-rank the results
					// For now, just return the vector results
					OnComplete.ExecuteIfBound(VectorResults, true);
				}
				else
				{
					OnComplete.ExecuteIfBound(TArray<FVectorSearchResult>(), false);
				}
			}));
		}
		else
		{
			OnComplete.ExecuteIfBound(TArray<FVectorSearchResult>(), false);
		}
	}));
}

// Private helper methods
TArray<float> UIGEDVectorDatabase::GenerateEmbedding(const FString& Text)
{
	// In a real implementation, this would call an embedding model API
	// For simulation, generate a random embedding with the correct dimensions
	TArray<float> Embedding;
	Embedding.Reserve(DatabaseConfig.EmbeddingDimensions);
	
	// Use text hash as seed for consistent "random" values
	uint32 Hash = GetTypeHash(Text);
	FRandomStream RandomStream(Hash);
	
	for (int32 i = 0; i < DatabaseConfig.EmbeddingDimensions; i++)
	{
		Embedding.Add(RandomStream.FRandRange(-1.0f, 1.0f));
	}
	
	// Normalize the vector
	float Magnitude = 0.0f;
	for (float Value : Embedding)
	{
		Magnitude += Value * Value;
	}
	Magnitude = FMath::Sqrt(Magnitude);
	
	if (Magnitude > 0.0f)
	{
		for (float& Value : Embedding)
		{
			Value /= Magnitude;
		}
	}
	
	return Embedding;
}

float UIGEDVectorDatabase::CalculateCosineSimilarity(const TArray<float>& VectorA, const TArray<float>& VectorB)
{
	if (VectorA.Num() != VectorB.Num())
	{
		return 0.0f;
	}
	
	float DotProduct = 0.0f;
	float MagnitudeA = 0.0f;
	float MagnitudeB = 0.0f;
	
	for (int32 i = 0; i < VectorA.Num(); i++)
	{
		DotProduct += VectorA[i] * VectorB[i];
		MagnitudeA += VectorA[i] * VectorA[i];
		MagnitudeB += VectorB[i] * VectorB[i];
	}
	
	MagnitudeA = FMath::Sqrt(MagnitudeA);
	MagnitudeB = FMath::Sqrt(MagnitudeB);
	
	if (MagnitudeA == 0.0f || MagnitudeB == 0.0f)
	{
		return 0.0f;
	}
	
	return DotProduct / (MagnitudeA * MagnitudeB);
}

TArray<FVectorSearchResult> UIGEDVectorDatabase::PerformSimilaritySearch(const TArray<float>& QueryEmbedding, int32 MaxResults)
{
	TArray<FVectorSearchResult> Results;
	
	for (const auto& VectorPair : VectorStorage)
	{
		const FVectorEmbedding& Embedding = VectorPair.Value;
		
		if (Embedding.Embedding.Num() != QueryEmbedding.Num())
		{
			continue;
		}
		
		float Similarity = CalculateCosineSimilarity(QueryEmbedding, Embedding.Embedding);
		
		if (Similarity >= DatabaseConfig.SimilarityThreshold)
		{
			FVectorSearchResult Result;
			Result.DocumentID = Embedding.DocumentID;
			Result.Content = Embedding.Content;
			Result.SimilarityScore = Similarity;
			Result.Metadata = Embedding.Metadata;
			Results.Add(Result);
		}
	}
	
	// Sort by similarity score (descending)
	Results.Sort([](const FVectorSearchResult& A, const FVectorSearchResult& B)
	{
		return A.SimilarityScore > B.SimilarityScore;
	});
	
	// Limit results
	if (Results.Num() > MaxResults)
	{
		Results.SetNum(MaxResults);
	}
	
	return Results;
}

void UIGEDVectorDatabase::UpdateIndex()
{
	// In a real implementation, this would update search indices
	// For now, just update the timestamp
	LastIndexUpdate = FDateTime::Now();
	UE_LOG(LogTemp, VeryVerbose, TEXT("Vector database index updated"));
}

void UIGEDVectorDatabase::SaveDatabaseToDisk()
{
	// In a real implementation, this would save the database to disk
	// For now, just log the action
	UE_LOG(LogTemp, VeryVerbose, TEXT("Saving vector database to disk..."));
}

void UIGEDVectorDatabase::LoadDatabaseFromDisk()
{
	// In a real implementation, this would load the database from disk
	// For now, just log the action
	UE_LOG(LogTemp, VeryVerbose, TEXT("Loading vector database from disk..."));
}

FString UIGEDVectorDatabase::GenerateDocumentID(const FString& Content)
{
	// Generate a unique document ID based on content hash and timestamp
	uint32 ContentHash = GetTypeHash(Content);
	FString Timestamp = FDateTime::Now().ToString();
	uint32 TimestampHash = GetTypeHash(Timestamp);
	
	return FString::Printf(TEXT("doc_%08x_%08x"), ContentHash, TimestampHash);
}

void UIGEDVectorDatabase::LogVectorDBEvent(const FString& Event, const FString& Details)
{
	UE_LOG(LogTemp, Log, TEXT("VectorDB Event: %s - %s"), *Event, *Details);
}

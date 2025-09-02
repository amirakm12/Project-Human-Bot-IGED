// IGEDEncryption.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IGEDEncryption.generated.h"

UCLASS(BlueprintType)
class IGED_API UIGEDEncryption : public UObject
{
    GENERATED_BODY()

public:
    UIGEDEncryption();

    // Singleton pattern
    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    static UIGEDEncryption* GetInstance();

    // Key Management
    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void GenerateKey();

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    bool LoadKeyFromFile(const FString& KeyFilePath);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    bool SaveKeyToFile(const FString& KeyFilePath);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    void RotateKey();

    // String Encryption
    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    FString EncryptString(const FString& PlainText);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    FString DecryptString(const FString& EncryptedText);

    // Data Encryption
    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    TArray<uint8> EncryptData(const TArray<uint8>& PlainData);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    TArray<uint8> DecryptData(const TArray<uint8>& EncryptedData);

    // File Encryption
    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    bool EncryptFile(const FString& InputFilePath, const FString& OutputFilePath);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    bool DecryptFile(const FString& InputFilePath, const FString& OutputFilePath);

    // Hashing
    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    FString HashString(const FString& Input);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    FString HashStringSHA256(const FString& Input);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    bool VerifyHash(const FString& Input, const FString& Hash);

    // Utilities
    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    FString GenerateRandomToken(int32 Length = 32);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    FString GenerateUUID();

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    bool SecureCompare(const FString& A, const FString& B);

    UFUNCTION(BlueprintCallable, Category = "IGED|Security")
    TArray<uint8> GetPublicKey() const;

private:
    static UIGEDEncryption* Instance;
    
    TArray<uint8> EncryptionKey;
    TArray<uint8> InitializationVector;
    
    int32 KeySize;
    int32 IVSize;
};
// IGEDEncryption.cpp

#include "Security/IGEDEncryption.h"
#include "Misc/Base64.h"
#include "Misc/SecureHash.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "IGED.h"

UIGEDEncryption* UIGEDEncryption::Instance = nullptr;

UIGEDEncryption::UIGEDEncryption()
{
    KeySize = 32; // 256-bit key
    IVSize = 16;  // 128-bit IV
    
    // Generate default key on initialization
    GenerateKey();
}

UIGEDEncryption* UIGEDEncryption::GetInstance()
{
    if (!Instance)
    {
        Instance = NewObject<UIGEDEncryption>();
        Instance->AddToRoot(); // Prevent garbage collection
    }
    return Instance;
}

void UIGEDEncryption::GenerateKey()
{
    // Generate random encryption key
    EncryptionKey.SetNum(KeySize);
    for (int32 i = 0; i < KeySize; i++)
    {
        EncryptionKey[i] = FMath::RandRange(0, 255);
    }
    
    // Generate random IV
    InitializationVector.SetNum(IVSize);
    for (int32 i = 0; i < IVSize; i++)
    {
        InitializationVector[i] = FMath::RandRange(0, 255);
    }
    
    UE_LOG(LogIGED, Warning, TEXT("Generated new encryption key"));
}

bool UIGEDEncryption::LoadKeyFromFile(const FString& KeyFilePath)
{
    TArray<uint8> FileData;
    
    if (FFileHelper::LoadFileToArray(FileData, *KeyFilePath))
    {
        if (FileData.Num() >= KeySize + IVSize)
        {
            // Extract key and IV from file
            EncryptionKey.Empty();
            EncryptionKey.Append(FileData.GetData(), KeySize);
            
            InitializationVector.Empty();
            InitializationVector.Append(FileData.GetData() + KeySize, IVSize);
            
            UE_LOG(LogIGED, Warning, TEXT("Loaded encryption key from file"));
            return true;
        }
    }
    
    UE_LOG(LogIGED, Error, TEXT("Failed to load encryption key from file"));
    return false;
}

bool UIGEDEncryption::SaveKeyToFile(const FString& KeyFilePath)
{
    TArray<uint8> FileData;
    FileData.Append(EncryptionKey);
    FileData.Append(InitializationVector);
    
    if (FFileHelper::SaveArrayToFile(FileData, *KeyFilePath))
    {
        UE_LOG(LogIGED, Warning, TEXT("Saved encryption key to file"));
        return true;
    }
    
    UE_LOG(LogIGED, Error, TEXT("Failed to save encryption key to file"));
    return false;
}

FString UIGEDEncryption::EncryptString(const FString& PlainText)
{
    if (PlainText.IsEmpty())
    {
        return FString();
    }
    
    // Convert string to bytes
    FTCHARToUTF8 Converter(*PlainText);
    TArray<uint8> PlainData((uint8*)Converter.Get(), Converter.Length());
    
    // Encrypt data
    TArray<uint8> EncryptedData = EncryptData(PlainData);
    
    // Convert to base64 for string representation
    return FBase64::Encode(EncryptedData);
}

FString UIGEDEncryption::DecryptString(const FString& EncryptedText)
{
    if (EncryptedText.IsEmpty())
    {
        return FString();
    }
    
    // Decode from base64
    TArray<uint8> EncryptedData;
    FBase64::Decode(EncryptedText, EncryptedData);
    
    // Decrypt data
    TArray<uint8> PlainData = DecryptData(EncryptedData);
    
    // Convert bytes to string
    FUTF8ToTCHAR Converter((const ANSICHAR*)PlainData.GetData(), PlainData.Num());
    return FString(Converter.Get());
}

TArray<uint8> UIGEDEncryption::EncryptData(const TArray<uint8>& PlainData)
{
    TArray<uint8> EncryptedData;
    
    // Simple XOR encryption with key (in production, use proper AES)
    EncryptedData.SetNum(PlainData.Num());
    
    for (int32 i = 0; i < PlainData.Num(); i++)
    {
        // XOR with key byte (cycling through key)
        uint8 KeyByte = EncryptionKey[i % EncryptionKey.Num()];
        uint8 IVByte = InitializationVector[i % InitializationVector.Num()];
        
        EncryptedData[i] = PlainData[i] ^ KeyByte ^ IVByte;
    }
    
    return EncryptedData;
}

TArray<uint8> UIGEDEncryption::DecryptData(const TArray<uint8>& EncryptedData)
{
    // XOR encryption is symmetric, so decryption is the same as encryption
    return EncryptData(EncryptedData);
}

FString UIGEDEncryption::HashString(const FString& Input)
{
    return FMD5::HashAnsiString(*Input);
}

FString UIGEDEncryption::HashStringSHA256(const FString& Input)
{
    FSHAHash Hash;
    FSHA1::HashBuffer(*Input, Input.Len() * sizeof(TCHAR), Hash.Hash);
    return Hash.ToString();
}

bool UIGEDEncryption::VerifyHash(const FString& Input, const FString& Hash)
{
    FString ComputedHash = HashString(Input);
    return ComputedHash.Equals(Hash, ESearchCase::IgnoreCase);
}

FString UIGEDEncryption::GenerateRandomToken(int32 Length)
{
    const FString CharacterSet = TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    FString Token;
    
    for (int32 i = 0; i < Length; i++)
    {
        int32 RandomIndex = FMath::RandRange(0, CharacterSet.Len() - 1);
        Token.AppendChar(CharacterSet[RandomIndex]);
    }
    
    return Token;
}

FString UIGEDEncryption::GenerateUUID()
{
    FGuid NewGuid = FGuid::NewGuid();
    return NewGuid.ToString();
}

bool UIGEDEncryption::SecureCompare(const FString& A, const FString& B)
{
    if (A.Len() != B.Len())
    {
        return false;
    }
    
    // Constant-time comparison to prevent timing attacks
    int32 Result = 0;
    for (int32 i = 0; i < A.Len(); i++)
    {
        Result |= A[i] ^ B[i];
    }
    
    return Result == 0;
}

void UIGEDEncryption::RotateKey()
{
    // Generate new key
    GenerateKey();
    
    // In production, you would re-encrypt all data with the new key
    UE_LOG(LogIGED, Warning, TEXT("Encryption key rotated"));
}

TArray<uint8> UIGEDEncryption::GetPublicKey() const
{
    // In production, this would return the public key for asymmetric encryption
    // For now, return a portion of the symmetric key (not secure, just for demo)
    TArray<uint8> PublicKey;
    PublicKey.Append(EncryptionKey.GetData(), FMath::Min(16, EncryptionKey.Num()));
    return PublicKey;
}

bool UIGEDEncryption::EncryptFile(const FString& InputFilePath, const FString& OutputFilePath)
{
    TArray<uint8> FileData;
    
    if (!FFileHelper::LoadFileToArray(FileData, *InputFilePath))
    {
        UE_LOG(LogIGED, Error, TEXT("Failed to load file for encryption: %s"), *InputFilePath);
        return false;
    }
    
    TArray<uint8> EncryptedData = EncryptData(FileData);
    
    if (!FFileHelper::SaveArrayToFile(EncryptedData, *OutputFilePath))
    {
        UE_LOG(LogIGED, Error, TEXT("Failed to save encrypted file: %s"), *OutputFilePath);
        return false;
    }
    
    UE_LOG(LogIGED, Warning, TEXT("File encrypted successfully"));
    return true;
}

bool UIGEDEncryption::DecryptFile(const FString& InputFilePath, const FString& OutputFilePath)
{
    TArray<uint8> EncryptedData;
    
    if (!FFileHelper::LoadFileToArray(EncryptedData, *InputFilePath))
    {
        UE_LOG(LogIGED, Error, TEXT("Failed to load encrypted file: %s"), *InputFilePath);
        return false;
    }
    
    TArray<uint8> DecryptedData = DecryptData(EncryptedData);
    
    if (!FFileHelper::SaveArrayToFile(DecryptedData, *OutputFilePath))
    {
        UE_LOG(LogIGED, Error, TEXT("Failed to save decrypted file: %s"), *OutputFilePath);
        return false;
    }
    
    UE_LOG(LogIGED, Warning, TEXT("File decrypted successfully"));
    return true;
}
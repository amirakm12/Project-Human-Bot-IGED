#pragma once

#include "CoreMinimal.h"
#include "Agents/IGEDBaseAgent.h"
#include "Core/IGEDModelManager.h"
#include "IGEDCodeGenAgent.generated.h"

UENUM(BlueprintType)
enum class ECodeLanguage : uint8
{
    CPlusPlus     UMETA(DisplayName = "C++"),
    Python        UMETA(DisplayName = "Python"),
    JavaScript    UMETA(DisplayName = "JavaScript"),
    TypeScript    UMETA(DisplayName = "TypeScript"),
    Java          UMETA(DisplayName = "Java"),
    CSharp        UMETA(DisplayName = "C#"),
    Go            UMETA(DisplayName = "Go"),
    Rust          UMETA(DisplayName = "Rust"),
    Swift         UMETA(DisplayName = "Swift"),
    Kotlin        UMETA(DisplayName = "Kotlin"),
    PHP           UMETA(DisplayName = "PHP"),
    Ruby          UMETA(DisplayName = "Ruby"),
    HTML          UMETA(DisplayName = "HTML"),
    CSS           UMETA(DisplayName = "CSS"),
    SQL           UMETA(DisplayName = "SQL"),
    Shell         UMETA(DisplayName = "Shell"),
    PowerShell    UMETA(DisplayName = "PowerShell"),
    UnrealScript  UMETA(DisplayName = "UnrealScript"),
    Blueprint     UMETA(DisplayName = "Blueprint"),
    HLSL          UMETA(DisplayName = "HLSL"),
    GLSL          UMETA(DisplayName = "GLSL")
};

UENUM(BlueprintType)
enum class ECodeTaskType : uint8
{
    Generate      UMETA(DisplayName = "Generate Code"),
    Debug         UMETA(DisplayName = "Debug Code"),
    Optimize      UMETA(DisplayName = "Optimize Code"),
    Refactor      UMETA(DisplayName = "Refactor Code"),
    Review        UMETA(DisplayName = "Code Review"),
    Test          UMETA(DisplayName = "Generate Tests"),
    Document      UMETA(DisplayName = "Generate Documentation"),
    Convert       UMETA(DisplayName = "Convert Language"),
    Analyze       UMETA(DisplayName = "Code Analysis"),
    Explain       UMETA(DisplayName = "Explain Code")
};

USTRUCT(BlueprintType)
struct FIGEDCodeRequest
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString RequestID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECodeLanguage Language;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECodeTaskType TaskType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ExistingCode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Requirements;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FString, FString> Parameters;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime Timestamp;

    FIGEDCodeRequest()
    {
        RequestID = FGuid::NewGuid().ToString();
        Description = TEXT("");
        Language = ECodeLanguage::CPlusPlus;
        TaskType = ECodeTaskType::Generate;
        Timestamp = FDateTime::Now();
    }
};

USTRUCT(BlueprintType)
struct FIGEDCodeResponse
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString RequestID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString GeneratedCode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Explanation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Suggestions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Warnings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float QualityScore;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDateTime ProcessingTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSuccess;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ErrorMessage;

    FIGEDCodeResponse()
    {
        RequestID = TEXT("");
        GeneratedCode = TEXT("");
        Explanation = TEXT("");
        QualityScore = 0.0f;
        ProcessingTime = FDateTime::Now();
        bSuccess = false;
        ErrorMessage = TEXT("");
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCodeGenerated, const FString&, RequestID, const FIGEDCodeResponse&, Response);

UCLASS(BlueprintType, Blueprintable)
class IGED_UE5_API UIGEDCodeGenAgent : public UIGEDBaseAgent
{
    GENERATED_BODY()

public:
    UIGEDCodeGenAgent();

    // UIGEDBaseAgent interface
    virtual bool ExecuteTask(const FString& TaskDescription, const TMap<FString, FString>& Parameters) override;
    virtual FString GetAgentName() const override;
    virtual FString GetStatus() const override;

    UFUNCTION(BlueprintCallable, Category = "IGED Code Generation")
    bool Initialize(UIGEDModelManager* ModelManager);

    UFUNCTION(BlueprintCallable, Category = "IGED Code Generation")
    void Shutdown();

    UFUNCTION(BlueprintCallable, Category = "IGED Code Generation")
    FString GenerateCode(const FIGEDCodeRequest& Request);

    UFUNCTION(BlueprintCallable, Category = "IGED Code Generation")
    bool GenerateCodeAsync(const FIGEDCodeRequest& Request);

    UFUNCTION(BlueprintCallable, Category = "IGED Code Generation")
    FString DebugCode(const FString& Code, ECodeLanguage Language, const FString& ErrorMessage = TEXT(""));

    UFUNCTION(BlueprintCallable, Category = "IGED Code Generation")
    FString OptimizeCode(const FString& Code, ECodeLanguage Language, const TArray<FString>& OptimizationGoals);

    UFUNCTION(BlueprintCallable, Category = "IGED Code Generation")
    FString RefactorCode(const FString& Code, ECodeLanguage Language, const FString& RefactoringGoal);

    UFUNCTION(BlueprintCallable, Category = "IGED Code Generation")
    FString ReviewCode(const FString& Code, ECodeLanguage Language);

    UFUNCTION(BlueprintCallable, Category = "IGED Code Generation")
    FString GenerateTests(const FString& Code, ECodeLanguage Language, const FString& TestFramework = TEXT(""));

    UFUNCTION(BlueprintCallable, Category = "IGED Code Generation")
    FString GenerateDocumentation(const FString& Code, ECodeLanguage Language, const FString& DocFormat = TEXT("markdown"));

    UFUNCTION(BlueprintCallable, Category = "IGED Code Generation")
    FString ConvertLanguage(const FString& Code, ECodeLanguage FromLanguage, ECodeLanguage ToLanguage);

    UFUNCTION(BlueprintCallable, Category = "IGED Code Generation")
    FString AnalyzeCode(const FString& Code, ECodeLanguage Language);

    UFUNCTION(BlueprintCallable, Category = "IGED Code Generation")
    FString ExplainCode(const FString& Code, ECodeLanguage Language);

    UFUNCTION(BlueprintCallable, Category = "IGED Code Generation")
    TArray<FString> GetSupportedLanguages() const;

    UFUNCTION(BlueprintCallable, Category = "IGED Code Generation")
    TArray<FString> GetSupportedFrameworks(ECodeLanguage Language) const;

    UFUNCTION(BlueprintCallable, Category = "IGED Code Generation")
    bool ValidateCode(const FString& Code, ECodeLanguage Language);

    UPROPERTY(BlueprintAssignable, Category = "IGED Code Generation")
    FOnCodeGenerated OnCodeGenerated;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    UIGEDModelManager* ModelManager;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, FIGEDCodeRequest> ActiveRequests;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<FString, FIGEDCodeResponse> RequestResults;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    FString ActiveModelName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    TMap<ECodeLanguage, TArray<FString>> LanguageFrameworks;

private:
    FCriticalSection RequestMutex;

    FString FormatCodePrompt(const FIGEDCodeRequest& Request);
    FString FormatDebugPrompt(const FString& Code, ECodeLanguage Language, const FString& ErrorMessage);
    FString FormatOptimizationPrompt(const FString& Code, ECodeLanguage Language, const TArray<FString>& Goals);
    FString FormatRefactoringPrompt(const FString& Code, ECodeLanguage Language, const FString& Goal);
    FString FormatReviewPrompt(const FString& Code, ECodeLanguage Language);
    FString FormatTestPrompt(const FString& Code, ECodeLanguage Language, const FString& Framework);
    FString FormatDocumentationPrompt(const FString& Code, ECodeLanguage Language, const FString& Format);
    FString FormatConversionPrompt(const FString& Code, ECodeLanguage FromLanguage, ECodeLanguage ToLanguage);
    FString FormatAnalysisPrompt(const FString& Code, ECodeLanguage Language);
    FString FormatExplanationPrompt(const FString& Code, ECodeLanguage Language);
    void InitializeLanguageFrameworks();
    void HandleModelResponse(const FString& ModelName, const FString& Response, bool bSuccess);
    bool ValidateCodeSyntax(const FString& Code, ECodeLanguage Language);
    float CalculateCodeQuality(const FString& Code, ECodeLanguage Language);
    TArray<FString> ExtractCodeSuggestions(const FString& Response);
    TArray<FString> ExtractWarnings(const FString& Response);
};

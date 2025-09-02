// IGEDPlayerController.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "IGEDPlayerController.generated.h"

UCLASS()
class IGED_API AIGEDPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AIGEDPlayerController();

    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    // Input handlers
    UFUNCTION()
    void OnVoiceInput();

    UFUNCTION()
    void OnExecuteCommand();

    UFUNCTION()
    void OnToggleMenu();

    UFUNCTION(BlueprintCallable, Category = "IGED")
    void SendCommand(const FString& Command);

    UFUNCTION(BlueprintCallable, Category = "IGED")
    void ProcessVoiceCommand(const FString& VoiceText);

protected:
    UPROPERTY(EditDefaultsOnly, Category = "IGED")
    TSubclassOf<class UUserWidget> MainMenuClass;

    UPROPERTY()
    class UUserWidget* MainMenuWidget;

    bool bIsMenuVisible;
};
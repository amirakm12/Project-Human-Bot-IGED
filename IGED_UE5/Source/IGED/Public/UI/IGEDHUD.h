// IGEDHUD.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "IGEDHUD.generated.h"

UCLASS()
class IGED_API AIGEDHUD : public AHUD
{
    GENERATED_BODY()

public:
    AIGEDHUD();

    virtual void DrawHUD() override;
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "IGED")
    void UpdatePerformanceMetrics(float FPS);

    UFUNCTION(BlueprintCallable, Category = "IGED")
    void ShowNotification(const FString& Message, float Duration = 3.0f);

    UFUNCTION(BlueprintCallable, Category = "IGED")
    void UpdateAIStatus(const FString& Status);

protected:
    // HUD Elements
    void DrawPerformanceMetrics();
    void DrawAIStatus();
    void DrawNotifications();
    void DrawAgentStatus();

    // Fonts
    UPROPERTY(EditDefaultsOnly, Category = "IGED")
    UFont* MainFont;

    UPROPERTY(EditDefaultsOnly, Category = "IGED")
    UFont* SmallFont;

private:
    float CurrentFPS;
    FString CurrentAIStatus;
    
    struct FNotification
    {
        FString Message;
        float TimeRemaining;
    };
    
    TArray<FNotification> ActiveNotifications;
};
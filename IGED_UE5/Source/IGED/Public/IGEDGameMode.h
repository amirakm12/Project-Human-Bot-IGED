// IGEDGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AI/IGEDAIController.h"
#include "IGEDGameMode.generated.h"

UCLASS()
class IGED_API AIGEDGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AIGEDGameMode();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION(BlueprintCallable, Category = "IGED")
    void ExecuteCommand(const FString& Command, const FString& Parameters);

    UFUNCTION(BlueprintCallable, Category = "IGED")
    TArray<AIGEDAIController*> GetActiveAgents() const { return ActiveAgents; }

protected:
    void SpawnAIAgents();
    void MainGameLoop();
    void UpdateGameState();

private:
    UPROPERTY()
    TArray<AIGEDAIController*> ActiveAgents;

    FTimerHandle MainLoopTimer;
};
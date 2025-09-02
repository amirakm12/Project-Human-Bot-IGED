// IGEDGameMode.cpp

#include "IGEDGameMode.h"
#include "Core/IGEDGameInstance.h"
#include "AI/IGEDAIController.h"
#include "UI/IGEDHUD.h"
#include "IGEDPlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "IGED.h"

AIGEDGameMode::AIGEDGameMode()
{
    // Set default pawn class
    DefaultPawnClass = nullptr;
    
    // Set default controller
    PlayerControllerClass = AIGEDPlayerController::StaticClass();
    
    // Set HUD
    HUDClass = AIGEDHUD::StaticClass();
    
    // Set AI Controller
    DefaultAIControllerClass = AIGEDAIController::StaticClass();
}

void AIGEDGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    UE_LOG(LogIGED, Warning, TEXT("IGED Game Mode Started"));
    
    // Initialize game instance
    if (UIGEDGameInstance* GameInstance = Cast<UIGEDGameInstance>(GetGameInstance()))
    {
        GameInstance->InitializeAISystem();
        GameInstance->InitializeVoiceRecognition();
        GameInstance->InitializeNetworking();
    }
    
    // Spawn AI agents
    SpawnAIAgents();
    
    // Start main game loop
    GetWorld()->GetTimerManager().SetTimer(
        MainLoopTimer,
        this,
        &AIGEDGameMode::MainGameLoop,
        0.016f, // 60 FPS
        true
    );
}

void AIGEDGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Cleanup
    GetWorld()->GetTimerManager().ClearTimer(MainLoopTimer);
    
    Super::EndPlay(EndPlayReason);
}

void AIGEDGameMode::SpawnAIAgents()
{
    UWorld* World = GetWorld();
    if (!World) return;
    
    // Spawn all 6 AI agents
    TArray<EAIAgentType> AgentTypes = {
        EAIAgentType::CodeGen,
        EAIAgentType::DataMiner,
        EAIAgentType::SecurityOps,
        EAIAgentType::NetworkIntel,
        EAIAgentType::RemoteControl,
        EAIAgentType::AdvancedSecOps
    };
    
    for (EAIAgentType AgentType : AgentTypes)
    {
        FVector SpawnLocation = FVector(0, 0, 0);
        FRotator SpawnRotation = FRotator::ZeroRotator;
        
        AActor* SpawnedAgent = World->SpawnActor<AActor>(
            AActor::StaticClass(),
            SpawnLocation,
            SpawnRotation
        );
        
        if (SpawnedAgent)
        {
            AIGEDAIController* AIController = World->SpawnActor<AIGEDAIController>(
                AIGEDAIController::StaticClass()
            );
            
            if (AIController)
            {
                AIController->InitializeAgent(AgentType);
                ActiveAgents.Add(AIController);
                
                UE_LOG(LogIGED, Warning, TEXT("Spawned AI Agent: %d"), (int32)AgentType);
            }
        }
    }
}

void AIGEDGameMode::MainGameLoop()
{
    // Process AI tasks
    for (AIGEDAIController* Agent : ActiveAgents)
    {
        if (Agent && Agent->IsValidLowLevel())
        {
            // Agent tick is handled by UE5
        }
    }
    
    // Update game state
    UpdateGameState();
}

void AIGEDGameMode::UpdateGameState()
{
    // Update performance metrics
    float FPS = 1.0f / GetWorld()->GetDeltaSeconds();
    
    // Broadcast to HUD
    if (AIGEDHUD* HUD = Cast<AIGEDHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD()))
    {
        HUD->UpdatePerformanceMetrics(FPS);
    }
}

void AIGEDGameMode::ExecuteCommand(const FString& Command, const FString& Parameters)
{
    UE_LOG(LogIGED, Warning, TEXT("Executing command: %s"), *Command);
    
    // Route to appropriate agent
    if (Command == TEXT("generate"))
    {
        if (ActiveAgents.IsValidIndex(0))
        {
            FAIAgentTask Task;
            Task.TaskName = TEXT("Code Generation");
            Task.AgentType = EAIAgentType::CodeGen;
            Task.Parameters = Parameters;
            ActiveAgents[0]->ExecuteTask(Task);
        }
    }
    else if (Command == TEXT("analyze"))
    {
        if (ActiveAgents.IsValidIndex(1))
        {
            FAIAgentTask Task;
            Task.TaskName = TEXT("Data Analysis");
            Task.AgentType = EAIAgentType::DataMiner;
            Task.Parameters = Parameters;
            ActiveAgents[1]->ExecuteTask(Task);
        }
    }
    else if (Command == TEXT("scan"))
    {
        if (ActiveAgents.IsValidIndex(2))
        {
            FAIAgentTask Task;
            Task.TaskName = TEXT("Security Scan");
            Task.AgentType = EAIAgentType::SecurityOps;
            Task.Parameters = Parameters;
            ActiveAgents[2]->ExecuteTask(Task);
        }
    }
}
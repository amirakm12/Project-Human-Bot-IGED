// IGEDHUD.cpp

#include "UI/IGEDHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "Kismet/GameplayStatics.h"
#include "IGEDGameMode.h"
#include "IGED.h"

AIGEDHUD::AIGEDHUD()
{
    // Load default fonts
    static ConstructorHelpers::FObjectFinder<UFont> MainFontObj(TEXT("/Engine/EngineFonts/Roboto"));
    if (MainFontObj.Succeeded())
    {
        MainFont = MainFontObj.Object;
    }
    
    static ConstructorHelpers::FObjectFinder<UFont> SmallFontObj(TEXT("/Engine/EngineFonts/RobotoTiny"));
    if (SmallFontObj.Succeeded())
    {
        SmallFont = SmallFontObj.Object;
    }
    
    CurrentFPS = 60.0f;
    CurrentAIStatus = TEXT("Initializing...");
}

void AIGEDHUD::BeginPlay()
{
    Super::BeginPlay();
    
    UE_LOG(LogIGED, Warning, TEXT("IGED HUD Initialized"));
}

void AIGEDHUD::DrawHUD()
{
    Super::DrawHUD();
    
    if (!Canvas) return;
    
    // Draw all HUD elements
    DrawPerformanceMetrics();
    DrawAIStatus();
    DrawNotifications();
    DrawAgentStatus();
}

void AIGEDHUD::DrawPerformanceMetrics()
{
    if (!MainFont) return;
    
    float X = 10.0f;
    float Y = 10.0f;
    
    // Draw FPS
    FString FPSText = FString::Printf(TEXT("FPS: %.1f"), CurrentFPS);
    FLinearColor FPSColor = CurrentFPS >= 60.0f ? FLinearColor::Green : 
                            CurrentFPS >= 30.0f ? FLinearColor::Yellow : 
                            FLinearColor::Red;
    
    Canvas->DrawText(MainFont, FPSText, X, Y, 1.0f, 1.0f, FCanvasTextItem::FontRenderInfo());
    
    // Draw Memory Usage
    Y += 25.0f;
    FString MemoryText = FString::Printf(TEXT("Memory: %.1f MB"), 
        FPlatformMemory::GetStats().UsedPhysical / (1024.0f * 1024.0f));
    Canvas->DrawText(MainFont, MemoryText, X, Y, 1.0f, 1.0f, FCanvasTextItem::FontRenderInfo());
    
    // Draw CPU Usage
    Y += 25.0f;
    FString CPUText = TEXT("CPU: Active");
    Canvas->DrawText(MainFont, CPUText, X, Y, 1.0f, 1.0f, FCanvasTextItem::FontRenderInfo());
}

void AIGEDHUD::DrawAIStatus()
{
    if (!MainFont) return;
    
    float X = Canvas->SizeX - 300.0f;
    float Y = 10.0f;
    
    // Draw AI Status Header
    FString HeaderText = TEXT("AI SYSTEM STATUS");
    Canvas->DrawText(MainFont, HeaderText, X, Y, 1.2f, 1.2f, FCanvasTextItem::FontRenderInfo());
    
    Y += 30.0f;
    Canvas->DrawText(SmallFont, CurrentAIStatus, X, Y, 1.0f, 1.0f, FCanvasTextItem::FontRenderInfo());
}

void AIGEDHUD::DrawNotifications()
{
    if (!MainFont) return;
    
    float X = Canvas->SizeX / 2.0f - 200.0f;
    float Y = 100.0f;
    
    // Update and draw notifications
    for (int32 i = ActiveNotifications.Num() - 1; i >= 0; i--)
    {
        FNotification& Notif = ActiveNotifications[i];
        Notif.TimeRemaining -= GetWorld()->GetDeltaSeconds();
        
        if (Notif.TimeRemaining <= 0)
        {
            ActiveNotifications.RemoveAt(i);
            continue;
        }
        
        float Alpha = FMath::Min(Notif.TimeRemaining, 1.0f);
        FLinearColor Color(1.0f, 1.0f, 1.0f, Alpha);
        
        Canvas->DrawText(MainFont, Notif.Message, X, Y, 1.0f, 1.0f, FCanvasTextItem::FontRenderInfo());
        Y += 30.0f;
    }
}

void AIGEDHUD::DrawAgentStatus()
{
    if (!SmallFont) return;
    
    float X = 10.0f;
    float Y = Canvas->SizeY - 200.0f;
    
    // Draw header
    Canvas->DrawText(MainFont, TEXT("AI AGENTS"), X, Y, 1.0f, 1.0f, FCanvasTextItem::FontRenderInfo());
    Y += 25.0f;
    
    // Get active agents
    if (AIGEDGameMode* GameMode = Cast<AIGEDGameMode>(GetWorld()->GetAuthGameMode()))
    {
        TArray<AIGEDAIController*> Agents = GameMode->GetActiveAgents();
        
        TArray<FString> AgentNames = {
            TEXT("Code Generator"),
            TEXT("Data Miner"),
            TEXT("Security Ops"),
            TEXT("Network Intel"),
            TEXT("Remote Control"),
            TEXT("Advanced SecOps")
        };
        
        for (int32 i = 0; i < AgentNames.Num(); i++)
        {
            FLinearColor StatusColor = (i < Agents.Num() && Agents[i]) ? 
                FLinearColor::Green : FLinearColor::Red;
            
            FString StatusText = FString::Printf(TEXT("[%s] %s"), 
                (i < Agents.Num() && Agents[i]) ? TEXT("ACTIVE") : TEXT("OFFLINE"),
                *AgentNames[i]);
            
            Canvas->DrawText(SmallFont, StatusText, X, Y, 1.0f, 1.0f, FCanvasTextItem::FontRenderInfo());
            Y += 20.0f;
        }
    }
}

void AIGEDHUD::UpdatePerformanceMetrics(float FPS)
{
    CurrentFPS = FPS;
}

void AIGEDHUD::ShowNotification(const FString& Message, float Duration)
{
    FNotification Notif;
    Notif.Message = Message;
    Notif.TimeRemaining = Duration;
    ActiveNotifications.Add(Notif);
    
    UE_LOG(LogIGED, Warning, TEXT("Notification: %s"), *Message);
}

void AIGEDHUD::UpdateAIStatus(const FString& Status)
{
    CurrentAIStatus = Status;
}
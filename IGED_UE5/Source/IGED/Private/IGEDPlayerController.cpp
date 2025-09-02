// IGEDPlayerController.cpp

#include "IGEDPlayerController.h"
#include "Core/IGEDGameInstance.h"
#include "IGEDGameMode.h"
#include "Blueprint/UserWidget.h"
#include "IGED.h"

AIGEDPlayerController::AIGEDPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
    bIsMenuVisible = false;
}

void AIGEDPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    UE_LOG(LogIGED, Warning, TEXT("IGED Player Controller Initialized"));
    
    // Create main menu widget
    if (MainMenuClass)
    {
        MainMenuWidget = CreateWidget<UUserWidget>(this, MainMenuClass);
        if (MainMenuWidget)
        {
            MainMenuWidget->AddToViewport();
            MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void AIGEDPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    // Bind input actions
    InputComponent->BindAction("VoiceInput", IE_Pressed, this, &AIGEDPlayerController::OnVoiceInput);
    InputComponent->BindAction("ExecuteCommand", IE_Pressed, this, &AIGEDPlayerController::OnExecuteCommand);
    InputComponent->BindAction("ToggleMenu", IE_Pressed, this, &AIGEDPlayerController::OnToggleMenu);
}

void AIGEDPlayerController::OnVoiceInput()
{
    UE_LOG(LogIGED, Warning, TEXT("Voice input activated"));
    
    if (UIGEDGameInstance* GameInstance = Cast<UIGEDGameInstance>(GetGameInstance()))
    {
        GameInstance->StartVoiceCapture();
    }
}

void AIGEDPlayerController::OnExecuteCommand()
{
    // Execute the current command
    SendCommand(TEXT("execute"));
}

void AIGEDPlayerController::OnToggleMenu()
{
    if (MainMenuWidget)
    {
        bIsMenuVisible = !bIsMenuVisible;
        MainMenuWidget->SetVisibility(bIsMenuVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
        
        // Update input mode
        if (bIsMenuVisible)
        {
            FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
            SetInputMode(InputMode);
        }
        else
        {
            SetInputMode(FInputModeGameOnly());
        }
    }
}

void AIGEDPlayerController::SendCommand(const FString& Command)
{
    UE_LOG(LogIGED, Warning, TEXT("Sending command: %s"), *Command);
    
    if (AIGEDGameMode* GameMode = Cast<AIGEDGameMode>(GetWorld()->GetAuthGameMode()))
    {
        GameMode->ExecuteCommand(Command, TEXT(""));
    }
}

void AIGEDPlayerController::ProcessVoiceCommand(const FString& VoiceText)
{
    UE_LOG(LogIGED, Warning, TEXT("Processing voice command: %s"), *VoiceText);
    
    if (UIGEDGameInstance* GameInstance = Cast<UIGEDGameInstance>(GetGameInstance()))
    {
        FString Intent = GameInstance->ProcessNaturalLanguage(VoiceText);
        SendCommand(Intent);
    }
}
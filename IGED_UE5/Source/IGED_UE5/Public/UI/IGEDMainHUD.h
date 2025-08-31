// IGED Main HUD - Advanced UI System with UMG/Slate
// Copyright (c) 2024 Project Human Bot

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "IGEDMainHUD.generated.h"

// Forward declarations
class UIGEDMainWidget;
class UIGEDTerminalWidget;
class UIGEDAgentControlWidget;
class UIGEDVisualizationWidget;
class UIGEDNetworkMapWidget;
class UCanvasPanel;
class UTextBlock;
class UProgressBar;
class UButton;
class UEditableTextBox;
class UScrollBox;
class UImage;
class UNiagaraComponent;

UENUM(BlueprintType)
enum class EHUDMode : uint8
{
	Terminal		UMETA(DisplayName = "Terminal Mode"),
	Visualization	UMETA(DisplayName = "3D Visualization"),
	NetworkMap		UMETA(DisplayName = "Network Map"),
	AgentControl	UMETA(DisplayName = "Agent Control"),
	Split			UMETA(DisplayName = "Split View")
};

USTRUCT(BlueprintType)
struct FHUDNotification
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Message;

	UPROPERTY(BlueprintReadWrite)
	FLinearColor Color;

	UPROPERTY(BlueprintReadWrite)
	float Duration;

	UPROPERTY(BlueprintReadWrite)
	UTexture2D* Icon;

	FHUDNotification()
	{
		Color = FLinearColor::White;
		Duration = 3.0f;
		Icon = nullptr;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHUDModeChanged, EHUDMode, NewMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCommandSubmitted, const FString&, Command);

UCLASS()
class IGED_UE5_API AIGEDMainHUD : public AHUD
{
	GENERATED_BODY()

public:
	AIGEDMainHUD();

	// AHUD interface
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;
	virtual void Tick(float DeltaTime) override;

	// UI Management
	UFUNCTION(BlueprintCallable, Category = "IGED|UI")
	void InitializeUI();

	UFUNCTION(BlueprintCallable, Category = "IGED|UI")
	void SetHUDMode(EHUDMode NewMode);

	UFUNCTION(BlueprintCallable, Category = "IGED|UI")
	EHUDMode GetCurrentMode() const { return CurrentMode; }

	// Terminal functions
	UFUNCTION(BlueprintCallable, Category = "IGED|Terminal")
	void AddTerminalOutput(const FString& Text, const FLinearColor& Color = FLinearColor::Green);

	UFUNCTION(BlueprintCallable, Category = "IGED|Terminal")
	void ClearTerminal();

	UFUNCTION(BlueprintCallable, Category = "IGED|Terminal")
	void ExecuteTerminalCommand(const FString& Command);

	// Visualization
	UFUNCTION(BlueprintCallable, Category = "IGED|Visualization")
	void UpdateVisualization(const TArray<float>& DataPoints);

	UFUNCTION(BlueprintCallable, Category = "IGED|Visualization")
	void SetVisualizationType(const FString& Type);

	UFUNCTION(BlueprintCallable, Category = "IGED|Visualization")
	void EnableCyberpunkEffects(bool bEnable);

	// Agent control
	UFUNCTION(BlueprintCallable, Category = "IGED|Agents")
	void UpdateAgentStatus(const FString& AgentName, bool bIsActive);

	UFUNCTION(BlueprintCallable, Category = "IGED|Agents")
	void ShowAgentDetails(const FString& AgentName);

	// Network map
	UFUNCTION(BlueprintCallable, Category = "IGED|Network")
	void AddNetworkNode(const FString& NodeID, const FVector2D& Position);

	UFUNCTION(BlueprintCallable, Category = "IGED|Network")
	void AddNetworkConnection(const FString& FromNode, const FString& ToNode);

	UFUNCTION(BlueprintCallable, Category = "IGED|Network")
	void HighlightNetworkPath(const TArray<FString>& NodePath);

	// Notifications
	UFUNCTION(BlueprintCallable, Category = "IGED|UI")
	void ShowNotification(const FHUDNotification& Notification);

	UFUNCTION(BlueprintCallable, Category = "IGED|UI")
	void ShowQuickNotification(const FString& Message, const FLinearColor& Color = FLinearColor::White);

	// System metrics display
	UFUNCTION(BlueprintCallable, Category = "IGED|UI")
	void UpdateSystemMetrics(float CPU, float Memory, float Network);

	UFUNCTION(BlueprintCallable, Category = "IGED|UI")
	void SetFPSDisplay(bool bShow);

	// Theme management
	UFUNCTION(BlueprintCallable, Category = "IGED|UI")
	void SetUITheme(const FString& ThemeName);

	UFUNCTION(BlueprintCallable, Category = "IGED|UI")
	void SetUIOpacity(float Opacity);

	// Animation controls
	UFUNCTION(BlueprintCallable, Category = "IGED|UI")
	void PlayIntroAnimation();

	UFUNCTION(BlueprintCallable, Category = "IGED|UI")
	void PlayTransition(EHUDMode FromMode, EHUDMode ToMode);

	// Events
	UPROPERTY(BlueprintAssignable, Category = "IGED|Events")
	FOnHUDModeChanged OnHUDModeChanged;

	UPROPERTY(BlueprintAssignable, Category = "IGED|Events")
	FOnCommandSubmitted OnCommandSubmitted;

protected:
	// Widget classes
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UIGEDMainWidget> MainWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UIGEDTerminalWidget> TerminalWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UIGEDAgentControlWidget> AgentControlWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UIGEDVisualizationWidget> VisualizationWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UIGEDNetworkMapWidget> NetworkMapWidgetClass;

	// Active widgets
	UPROPERTY()
	UIGEDMainWidget* MainWidget;

	UPROPERTY()
	UIGEDTerminalWidget* TerminalWidget;

	UPROPERTY()
	UIGEDAgentControlWidget* AgentControlWidget;

	UPROPERTY()
	UIGEDVisualizationWidget* VisualizationWidget;

	UPROPERTY()
	UIGEDNetworkMapWidget* NetworkMapWidget;

	// Current state
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	EHUDMode CurrentMode;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	bool bShowFPS;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	float UIOpacity;

	// Notifications
	TArray<FHUDNotification> ActiveNotifications;
	TArray<float> NotificationTimers;

	// Metrics
	float CurrentFPS;
	float CPUUsage;
	float MemoryUsage;
	float NetworkUsage;

	// Theme
	FString CurrentTheme;
	TMap<FString, FLinearColor> ThemeColors;

private:
	void CreateWidgets();
	void SetupDefaultThemes();
	void UpdateNotifications(float DeltaTime);
	void DrawSystemMetrics();
	void DrawCyberpunkGrid();
	void ApplyTheme(const FString& ThemeName);
	
	// Cyberpunk visual effects
	void DrawScanlines();
	void DrawGlitchEffect();
	void DrawHolographicBorder();
	
	// Animation state
	float IntroAnimationTime;
	float TransitionAnimationTime;
	bool bIsAnimating;
	
	// Grid effect parameters
	float GridPulseTime;
	float GridIntensity;
	FLinearColor GridColor;
};

// IGED.Build.cs

using UnrealBuildTool;

public class IGED : ModuleRules
{
    public IGED(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(new string[] { 
            "Core", 
            "CoreUObject", 
            "Engine", 
            "InputCore",
            "HeadMountedDisplay",
            "NavigationSystem",
            "AIModule",
            "Niagara",
            "EnhancedInput",
            "HTTP",
            "Json",
            "JsonUtilities",
            "WebSockets",
            "Voice",
            "AudioCapture",
            "SignalProcessing",
            "AudioMixer",
            "Networking",
            "Sockets",
            "OnlineSubsystem",
            "OnlineSubsystemUtils"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Slate",
            "SlateCore",
            "UMG",
            "GameplayTasks",
            "GameplayTags",
            "GameplayAbilities",
            "DeveloperSettings"
        });

        // Add AI and ML modules
        if (Target.bBuildDeveloperTools || Target.Configuration != UnrealTargetConfiguration.Shipping)
        {
            PrivateDependencyModuleNames.Add("GameplayDebugger");
        }

        // Enable exceptions for complex AI logic
        bEnableExceptions = true;
        
        // Enable RTTI for AI systems
        bUseRTTI = true;
    }
}
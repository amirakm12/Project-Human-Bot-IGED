// IGED UE5 Module Build Configuration
// Copyright (c) 2024 Project Human Bot

using UnrealBuildTool;

public class IGED_UE5 : ModuleRules
{
	public IGED_UE5(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		// Core dependencies
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"EnhancedInput",
			"HTTP",
			"Json",
			"JsonUtilities",
			"Niagara",
			"NiagaraCore",
			"AudioCapture",
			"AudioMixer",
			"UMG",
			"Slate",
			"SlateCore",
			"RenderCore",
			"RHI",
			"Projects",
			"Sockets",
			"Networking",
			"ApplicationCore",
			"PlatformFilemanager"
		});
		
		// Private dependencies for advanced features
		PrivateDependencyModuleNames.AddRange(new string[] {
			"ToolMenus",
			"EditorSubsystem",
			"UnrealEd",
			"EngineSettings",
			"ImageWrapper",
			"Voice",
			"AudioCaptureCore"
		});
		
		// Windows-specific libraries for biometric authentication
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicAdditionalLibraries.Add("winhttp.lib");
			PublicAdditionalLibraries.Add("bcrypt.lib");
			PublicAdditionalLibraries.Add("ncrypt.lib");
			PublicAdditionalLibraries.Add("crypt32.lib");
			PublicAdditionalLibraries.Add("secur32.lib");
			PublicAdditionalLibraries.Add("userenv.lib");
		}
		
		// Enable C++20 features
		CppStandard = CppStandardVersion.Cpp20;
		
		// Optimization
		OptimizeCode = CodeOptimization.InShipping;
		
		// Include paths
		PublicIncludePaths.AddRange(new string[] {
			"IGED_UE5/Public",
			"IGED_UE5/Public/Core",
			"IGED_UE5/Public/Agents",
			"IGED_UE5/Public/UI",
			"IGED_UE5/Public/Networking",
			"IGED_UE5/Public/Security",
			"IGED_UE5/Public/Visualization"
		});
		
		PrivateIncludePaths.AddRange(new string[] {
			"IGED_UE5/Private",
			"IGED_UE5/Private/Core",
			"IGED_UE5/Private/Agents",
			"IGED_UE5/Private/UI",
			"IGED_UE5/Private/Networking",
			"IGED_UE5/Private/Security",
			"IGED_UE5/Private/Visualization"
		});
	}
}

// IGED UE5 - Advanced AI Assistant Target Configuration
// Copyright (c) 2024 Project Human Bot

using UnrealBuildTool;
using System.Collections.Generic;

public class IGED_UE5Target : TargetRules
{
	public IGED_UE5Target(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("IGED_UE5");
		
		// Enable advanced features
		bUseUnityBuild = true;
		bUsePCHFiles = true;
		bUseAdaptiveUnityBuild = true;
		MinFilesUsingPrecompiledHeader = 2;
		bFasterWithoutUnity = false;
		
		// Optimization settings
		if (Configuration == UnrealTargetConfiguration.Shipping)
		{
			bUseLoggingInShipping = false;
			bUseChecksInShipping = false;
		}
	}
}

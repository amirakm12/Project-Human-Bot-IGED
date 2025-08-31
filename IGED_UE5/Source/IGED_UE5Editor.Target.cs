// IGED UE5 Editor Target Configuration
// Copyright (c) 2024 Project Human Bot

using UnrealBuildTool;
using System.Collections.Generic;

public class IGED_UE5EditorTarget : TargetRules
{
	public IGED_UE5EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("IGED_UE5");
		
		// Enable editor features
		bBuildWithEditorOnlyData = true;
		bCompileAgainstEditor = true;
		bCompileWithAccessibilitySupport = true;
		bWithLiveCoding = true;
	}
}

// IGEDEditor.Target.cs

using UnrealBuildTool;
using System.Collections.Generic;

public class IGEDEditorTarget : TargetRules
{
    public IGEDEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V4;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
        ExtraModuleNames.Add("IGED");
        
        // Enable editor features
        bBuildDeveloperTools = true;
        bCompileAgainstEngine = true;
        bCompileAgainstCoreUObject = true;
        bCompileAgainstUnrealEd = true;
        bBuildWithEditorOnlyData = true;
    }
}
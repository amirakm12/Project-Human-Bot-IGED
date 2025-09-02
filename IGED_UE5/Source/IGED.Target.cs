// IGED.Target.cs

using UnrealBuildTool;
using System.Collections.Generic;

public class IGEDTarget : TargetRules
{
    public IGEDTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V4;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
        ExtraModuleNames.Add("IGED");
        
        // Enable all advanced features
        bUseLoggingInShipping = true;
        bUseChecksInShipping = true;
        bCompileAgainstEngine = true;
        bCompileAgainstCoreUObject = true;
        bCompileAgainstApplicationCore = true;
        
        // AI and networking optimizations
        bWithServerCode = true;
        bCompilePhysX = true;
        bCompileAPEX = true;
        bCompileNvCloth = true;
        bCompileICU = true;
    }
}
using UnrealBuildTool;
using System.Collections.Generic;

public class HelloNeighborUE5EditorTarget : TargetRules
{
	public HelloNeighborUE5EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("HelloNeighborUE5");
	}
}

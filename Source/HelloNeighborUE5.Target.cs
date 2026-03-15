using UnrealBuildTool;
using System.Collections.Generic;

public class HelloNeighborUE5Target : TargetRules
{
	public HelloNeighborUE5Target(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("HelloNeighborUE5");
	}
}

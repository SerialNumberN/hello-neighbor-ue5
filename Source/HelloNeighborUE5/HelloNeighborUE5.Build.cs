using UnrealBuildTool;

public class HelloNeighborUE5 : ModuleRules
{
	public HelloNeighborUE5(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "PhysicsCore", "AIModule", "GameplayTasks", "NavigationSystem" });
	}
}

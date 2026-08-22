using UnrealBuildTool;

public class AIP : ModuleRules
{
	public AIP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"UMG",
			"Slate",
			"SlateCore",
			"Json",
			"JsonUtilities",
			"HTTP"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});
	}
}

// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ROGame : ModuleRules
{
	public ROGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"Niagara",
            "AnimGraphRuntime"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ROGame",
			"ROGame/Variant_Platforming",
			"ROGame/Variant_Platforming/Animation",
			"ROGame/Variant_Combat",
			"ROGame/Variant_Combat/AI",
			"ROGame/Variant_Combat/Animation",
			"ROGame/Variant_Combat/Gameplay",
			"ROGame/Variant_Combat/Interfaces",
			"ROGame/Variant_Combat/UI",
			"ROGame/Variant_SideScrolling",
			"ROGame/Variant_SideScrolling/AI",
			"ROGame/Variant_SideScrolling/Gameplay",
			"ROGame/Variant_SideScrolling/Interfaces",
			"ROGame/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EldenRing_Mod : ModuleRules
{
	public EldenRing_Mod(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
			"UMG","Slate", "SlateCore", "AnimGraphRuntime",
        "AIModule", "GameplayTasks", "AnimationModifiers", "AnimationDataController",
		"AnimationBlueprintLibrary", "NavigationSystem"});
	}
}

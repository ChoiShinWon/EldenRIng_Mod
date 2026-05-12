// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EldenRIng_Mod : ModuleRules
{
	public EldenRIng_Mod(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
			"UMG","Slate", "SlateCore", "AnimGraphRuntime",
        "AIModule", "GameplayTasks"});
	}
}

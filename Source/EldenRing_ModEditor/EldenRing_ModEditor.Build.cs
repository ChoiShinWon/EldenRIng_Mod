// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

// 에디터 전용 모듈 - 애니메이션 모디파이어처럼 UnrealEd에 의존하는 코드는 게임 모듈에 둘 수 없다.
// 게임(패키징) 타깃은 이 모듈을 아예 모르고, 에디터 타깃에서만 EldenRing_ModEditor.Target.cs가 불러온다.
public class EldenRing_ModEditor : ModuleRules
{
	public EldenRing_ModEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine",
			"AnimationModifiers", "AnimationDataController", "AnimationBlueprintLibrary" });
	}
}

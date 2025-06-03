// Copyright June Rhodes. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

public class GameManagementFrameworkEditor : ModuleRules
{
	public GameManagementFrameworkEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		// Ensure we're always using the latest build and include order versions, so that our
		// CI/CD server can pick up issues in new engine releases.
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"UnrealEd",
				"Kismet",
				"KismetCompiler",
				"BlueprintGraph",
				"LevelEditor",
				"EditorStyle",
				"SlateCore",
				"Slate",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Core",
				"GameManagementFramework",
				"DeveloperSettings",
			}
		);
	}
}
// Copyright June Rhodes. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

public class GameManagementFramework : ModuleRules
{
    public GameManagementFramework(ReadOnlyTargetRules Target) : base(Target)
    {
		// Ensure we're always using the latest build and include order versions, so that our
		// CI/CD server can pick up issues in new engine releases.
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

#if UE_5_0_OR_LATER
        PublicDefinitions.Add("UE_5_0_OR_LATER=1");
#endif

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
#if UE_4_26_OR_LATER
                "DeveloperSettings",
#endif
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Core",
            }
        );
    }
}
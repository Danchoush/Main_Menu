// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Lab4 : ModuleRules
{
	public Lab4(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", 
			"HeadMountedDisplay", "UMG", "OnlineSubsystem", "XmlParser" });
	}
}

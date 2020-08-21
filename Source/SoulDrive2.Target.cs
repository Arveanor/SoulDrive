// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class SoulDrive2Target : TargetRules
{
	public SoulDrive2Target(TargetInfo Target) : base (Target)
	{
		Type = TargetType.Game;

        ExtraModuleNames.Add("SoulDrive2");
    }
}

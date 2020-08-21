// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class SoulDrive2EditorTarget : TargetRules
{
	public SoulDrive2EditorTarget(TargetInfo Target) : base (Target)
	{
		Type = TargetType.Editor;

        ExtraModuleNames.Add("SoulDrive2");
	}
}

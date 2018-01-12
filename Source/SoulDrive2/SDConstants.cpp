// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDConstants.h"

SDConstants::SDConstants()
{
}

SDConstants::~SDConstants()
{
}

const FName SDConstants::DEBUG_ACTION_COMMAND = FName("DebugAction");
const bool SDConstants::CheatMode = true;

const TSet<FName> SDConstants::SpellNames { FName("DeveloperAngstSpell"), FName("CheatSpell") };
const FName SDConstants::DEVELOPER_ANGST = FName("DeveloperAngstSpell");
const FName SDConstants::CHEAT_SPELL = FName("CheatSpell");
const FName SDConstants::NO_SPELL = FName("SpellNone");
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Set.h"
/**
 * 
 */
class SOULDRIVE2_API SDConstants
{
public:
	SDConstants();
	~SDConstants();

	static const FName DEBUG_ACTION_COMMAND;
	static const enum HotKeyOverrides { NO_ACTION_WRITABLE = 0, DEBUG_ACTION_WRITABLE = 1, CHEAT_SPELL_WRITABLE = 1 } HOTKEY_OVERRIDES;

	static const bool CheatMode;

	static const TSet<FName> SpellNames;

	//spell fnames
	static const FName NO_SPELL;
	static const FName DEVELOPER_ANGST;
	static const FName CHEAT_SPELL;
};

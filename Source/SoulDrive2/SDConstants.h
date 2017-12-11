// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
class SOULDRIVE2_API SDConstants
{
public:
	SDConstants();
	~SDConstants();

	static const FName DEBUG_ACTION_COMMAND;
	static const enum HotKeyOverrides { NO_ACTION_WRITABLE = 0, DEBUG_ACTION_WRITABLE = 1 } HOTKEY_OVERRIDES;

	//spell fnames
	static const FName DEVELOPER_ANGST;
};

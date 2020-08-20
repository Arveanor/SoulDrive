// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "SDPlayerState.h"
#include "SDReconnectSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API USDReconnectSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	FString SaveSlotName;

	UPROPERTY()
	FURL HostURL;
	
	USDReconnectSaveGame();
};

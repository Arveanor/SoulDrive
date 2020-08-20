// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "SDPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	ASDPlayerState(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, Replicated)
	uint8 DebugKey;

	UPROPERTY(Replicated)
	FURL HostAddr;

};

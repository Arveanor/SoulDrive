// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"
#include "SDTeamIdentity.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class USDTeamIdentity : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class SOULDRIVE2_API ISDTeamIdentity
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Team")
	int GetTeamId();
	
};

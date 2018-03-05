// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SDFinalCall.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USDFinalCall : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class SOULDRIVE2_API ISDFinalCall
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	void MakeLastCall();
	
};

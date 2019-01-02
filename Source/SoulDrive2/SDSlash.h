// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SDBaseSpell.h"
#include "SDSlash.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDSlash : public ASDBaseSpell
{
	GENERATED_BODY()
	
	public:
	virtual void CastSpell(FVector target);
	virtual void BeginPlay() override;

private:
	const uint8 NUM_TRACES = 5;
	const float TRACE_LENGTH = 225.f; // length of each trace line
	const float TRACE_ANGLE = 15.f; // angle between each trace line

	/* Figure out where the first vector should be drawn for our hit check, this should probably be in a different class once it needs to be duplicated*/
	float GetInitialRotation();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SDBaseSpell.h"
#include "SDSunBurstSpell.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDSunBurstSpell : public ASDBaseSpell
{
	GENERATED_BODY()
	
public:
	virtual void CastSpell(FVector target);
	virtual void BeginPlay() override;
	virtual void HandleTarget(AActor *Target, bool IsAlly) override;

};

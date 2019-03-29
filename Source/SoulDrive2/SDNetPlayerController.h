// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "SDNetPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDNetPlayerController : public AAIController
{
	GENERATED_BODY()
	
public:
	ASDNetPlayerController();
	bool MoveToLocation(FVector &Goal);
	virtual void Tick(float DeltaTime) override;

private:
	void SwapWeapons();
};

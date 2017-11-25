// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "SDPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	bool bMoveToCursor;

	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
	void MoveToCursor();

public:

	ASDPlayerController();
	
};
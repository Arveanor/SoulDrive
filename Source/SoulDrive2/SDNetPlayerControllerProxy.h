// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "SDNetPlayerController.h"
#include "SDNetPlayerPawn.h"
#include "SDNetPlayerProxy.h"
#include "SDNetPlayerControllerProxy.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDNetPlayerControllerProxy : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	
public:
	ASDNetPlayerControllerProxy();

	UFUNCTION(Server, Reliable, WithValidation)
	void MoveToLocation(FHitResult HitResult);
	void MoveToLocation_Implementation(FHitResult HitResult);
	bool MoveToLocation_Validate(FHitResult HitResult);
	
private:

	void OnMovementKeyPressed();
	void OnMovementKeyReleased();

	// Server side character and controller to handle real logic for this proxy
	ASDNetPlayerController *ServerController;
	ASDNetPlayerPawn *ServerCharacter;
	ASDNetPlayerProxy *PlayerProxy;
	
	bool bMoveToLocation;
	
};

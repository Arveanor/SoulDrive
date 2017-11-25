// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDPlayerPawn.h"


ASDPlayerPawn::ASDPlayerPawn()
{

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom...
	MainCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	MainCameraBoom->SetupAttachment(RootComponent);
	MainCameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	MainCameraBoom->TargetArmLength = 2800.f;
	MainCameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	MainCameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

											  // Create a camera...
	PlayerCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	PlayerCameraComponent->SetupAttachment(MainCameraBoom, USpringArmComponent::SocketName);
	PlayerCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}
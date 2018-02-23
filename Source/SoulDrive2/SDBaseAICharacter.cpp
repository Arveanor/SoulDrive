// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDBaseAICharacter.h"


// Sets default values
ASDBaseAICharacter::ASDBaseAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = ASDBaseAIController::StaticClass();
}

// Called when the game starts or when spawned
void ASDBaseAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASDBaseAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASDBaseAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


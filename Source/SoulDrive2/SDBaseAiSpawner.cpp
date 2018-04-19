// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDBaseAiSpawner.h"


// Sets default values
ASDBaseAiSpawner::ASDBaseAiSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASDBaseAiSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASDBaseAiSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


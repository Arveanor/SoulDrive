// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDBaseAoE.h"


// Sets default values
ASDBaseAoE::ASDBaseAoE()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASDBaseAoE::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASDBaseAoE::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


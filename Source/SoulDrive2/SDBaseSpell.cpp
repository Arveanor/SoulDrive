// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDBaseSpell.h"


// Sets default values
ASDBaseSpell::ASDBaseSpell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASDBaseSpell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASDBaseSpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


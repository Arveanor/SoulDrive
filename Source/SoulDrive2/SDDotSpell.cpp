// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDDotSpell.h"


// Sets default values
ASDDotSpell::ASDDotSpell(const class FObjectInitializer& FOI)
	: Super(FOI)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASDDotSpell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASDDotSpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


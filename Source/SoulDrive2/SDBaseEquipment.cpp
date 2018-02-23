// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDBaseEquipment.h"


// Sets default values
ASDBaseEquipment::ASDBaseEquipment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASDBaseEquipment::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASDBaseEquipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDNetPlayerPawn.h"




void ASDNetPlayerPawn::TravelToLevel(FName LevelToLoad)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempting to do ServerTravel"));
	if (HasAuthority())
	{
		GetWorld()->ServerTravel(LevelToLoad.ToString());
	}
}

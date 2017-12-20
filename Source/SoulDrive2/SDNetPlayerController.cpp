// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDNetPlayerController.h"



void ASDNetPlayerController::MoveToLocation(FVector &Goal)
{
	UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
	NavSys->SimpleMoveToLocation(this, Goal);
}

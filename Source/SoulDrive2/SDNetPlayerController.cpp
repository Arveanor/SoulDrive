// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDNetPlayerController.h"
#include "NavigationSystem.h"
#include "SDNetPlayerPawn.h"

ASDNetPlayerController::ASDNetPlayerController()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

bool ASDNetPlayerController::MoveToLocation(FVector &Goal)
{
	if (GetWorld() != nullptr)
	{
		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		//UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		ASDNetPlayerPawn* ControlledPawn = dynamic_cast<ASDNetPlayerPawn *>(GetPawn());
		if (ControlledPawn != nullptr && !ControlledPawn->IsCasting())
		{
			ControlledPawn->IsMoving = true;
			NavSys->SimpleMoveToLocation(this, Goal);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void ASDNetPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		ASDNetPlayerPawn* ControlledPawn = dynamic_cast<ASDNetPlayerPawn *>(GetPawn());
		if (ControlledPawn != nullptr)
		{
			ControlledPawn->IsMoving = false;
		}
	}
}


void ASDNetPlayerController::SwapWeapons()
{
	ASDNetPlayerPawn* ControlledPawn = dynamic_cast<ASDNetPlayerPawn *>(GetPawn());
	if (ControlledPawn != nullptr)
	{
		ControlledPawn->SwapWeapons();
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDNetPlayerPawn.h"




ASDNetPlayerPawn::ASDNetPlayerPawn()
{
	this->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ASDNetPlayerPawn::OnOverlapBegin);
}

void ASDNetPlayerPawn::TravelToLevel(FName LevelToLoad)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempting to do ServerTravel"));
	if (HasAuthority())
	{
		bool result = GetWorld()->ServerTravel(LevelToLoad.ToString());
		if (result)
		{
			UE_LOG(LogTemp, Warning, TEXT("returned true"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("returned false"));
		}
	}
}

void ASDNetPlayerPawn::EquipItem(FString MeshName)
{

}

void ASDNetPlayerPawn::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Server pawn overlap event!"));
}

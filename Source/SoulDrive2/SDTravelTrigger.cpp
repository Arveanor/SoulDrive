// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDTravelTrigger.h"
#include "DrawDebugHelpers.h"
#include "SDNetPlayerPawn.h"
#include "SDConstants.h"


void ASDTravelTrigger::BeginPlay()
{
	Super::BeginPlay();
	if (SDConstants::CheatMode)
	{
		DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Purple, true, -1, 0, 5);
	}
}

ASDTravelTrigger::ASDTravelTrigger()
{
	LevelToLoad = FName(TEXT("CityPersistent"));
	OnActorBeginOverlap.AddDynamic(this, &ASDTravelTrigger::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ASDTravelTrigger::OnOverlapEnd);
}

void ASDTravelTrigger::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	ASDNetPlayerPawn *PlayerPawn = dynamic_cast<ASDNetPlayerPawn *>(OtherActor);
	if (PlayerPawn != nullptr)
	{
		PlayerPawn->TravelToLevel(LevelToLoad);
	}
}

void ASDTravelTrigger::OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor)
{

}

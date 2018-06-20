// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDNetPlayerControllerProxy.h"
#include "SDCommonUtilities.h"


// Sets default values
ASDCommonUtilities::ASDCommonUtilities()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASDCommonUtilities::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASDCommonUtilities::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASDCommonUtilities::AllControllersPostLevelLoad(UObject *WorldContextObject)
{
	if (WorldContextObject->GetWorld() != nullptr)
	{
		for (TActorIterator<ASDNetPlayerControllerProxy> ControllerItr(WorldContextObject->GetWorld()); ControllerItr; ++ControllerItr)
		{
			ControllerItr->HandleLevelLoaded();
		}
	}
}


// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include "SoulDrive2GameModeBase.h"


AActor* ASoulDrive2GameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	AActor* result = nullptr;
	if (playerStartArray.Num() == 0)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), playerStartArray);
	}
	for (AActor *PStart : playerStartArray)
	{
		if (PStart->ActorHasTag(FName("player1"))
			&& Player->GetName().Compare(FString("SharedPlayerController_C_0")))
		{
			return PStart;
		}
		else if (PStart->ActorHasTag(FName("player2"))
			&& Player->GetName().Compare(FString("SharedPlayerController_C_1")))
		{
			return PStart;
		}
	}
	if (playerStartArray.Num() > 0)
	{
		result = playerStartArray.Pop(true);
	}

	return result;
}


void ASoulDrive2GameModeBase::GenerateMapData(UPARAM(ref) TArray< TSubclassOf<AActor> > &TileList, FMapGenerationParams Params)
{
	for (int i = 0; i < Params.TileCountX; i++)
	{
		for (int j = 0; j < Params.TileCountY; j++)
		{
			TileList.Add(Params.ActorTiles[FMath::RandRange(0, Params.ActorTiles.Num() - 1)]);
		}
	}
}

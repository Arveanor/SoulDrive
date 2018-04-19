// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include "SDNetPlayerProxy.h"
#include "SDNetPlayerControllerProxy.h"
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
	int testIndex = -1;
	int randMax = 0;
	int randStart = 0;
	int attempts = 0;
	TArray<int> CriticalIndicesList;

	for (int i = 0; i < Params.TileCountX; i++)
	{
		for (int j = 0; j < Params.TileCountY; j++)
		{
			TileList.Add(Params.ActorTiles[FMath::RandRange(0, Params.ActorTiles.Num() - 1)]);
		}
	}

	// Write over locations in tile list. Store those indices in CriticalIndicesList to avoid collisions.
	for (FMapGenActorPair ActorPair : Params.ExactlyNActors)
	{
		for (int i = 0; i < ActorPair.Frequency; i++)
		{
			attempts = 0;
			randMax = TileList.Num() - 1;
			while (!(testIndex >= 0 && testIndex < randMax) && attempts < 10)
			{
				randStart = FMath::RandRange(0, randMax);
				for (testIndex = randStart; CriticalIndicesList.Contains(testIndex); testIndex++);

				// if our location went greater than TileList length, we'll want to re-roll a lower value than the last one we tried.
				randMax = randStart - 1;
				attempts++; // don't want to somehow get stuck in her endlessly!

				UE_LOG(LogTemp, Error, TEXT("could not find a location for the next required tile!"));
			}
			CriticalIndicesList.Add(testIndex);
			TileList[testIndex] = ActorPair.ActorTile;
		}
	}
}

void ASoulDrive2GameModeBase::SpawnPlayerCharacter(APlayerController* Player) 
{
	AActor* SpawnAt = ChoosePlayerStart_Implementation(Player);
	APawn* SpawnedCharacter;
	ASDNetPlayerControllerProxy *ProxyController = dynamic_cast<ASDNetPlayerControllerProxy *>(Player);

	FActorSpawnParameters SpawnParams;
	if (SpawnAt != nullptr)
	{
		SpawnedCharacter = GetWorld()->SpawnActor<ASDNetPlayerProxy>(SpawnAt->GetActorLocation(), SpawnAt->GetActorRotation(), SpawnParams);
	} 
	else
	{
		SpawnedCharacter = GetWorld()->SpawnActor<ASDNetPlayerProxy>(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnParams);
	}

	if (SpawnedCharacter != nullptr)
	{
		if (ProxyController != nullptr)
		{
			ProxyController->SetControllerInputModeGame();
		}
		Player->Possess(SpawnedCharacter);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn character!"));
	}
}

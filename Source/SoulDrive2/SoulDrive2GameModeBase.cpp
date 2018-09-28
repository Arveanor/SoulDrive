// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include "SDNetPlayerProxy.h"
#include "SDNetPlayerControllerProxy.h"
#include "SoulDrive2GameModeBase.h"
#include "List.h"


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


void ASoulDrive2GameModeBase::GenerateMapData(UPARAM(ref) TArray<USDTileDescriptor*> &TileList, FMapGenerationParams Params)
{
	int RandomIndex = 0;
	/* 
	** Array of indices not yet overwritten by a "necessary" tile.  Will be filled in with all
	** of the possible indices initially, and those elements will be removed when a needed tile is added on the second pass
	*/
	TArray<int> availableIndices;
	TArray<USDTileDescriptor *> possibleTilesForMap; // 0 as nullptr
	TSet<USDTileDescriptor *> candidateTiles;
	TArray<ProceduralTileEdges> neighborEdges;
	TArray<USDTileDescriptor *> neighborTiles;
	TileList.Init(0, Params.TileCountX * Params.TileCountY);
	availableIndices.SetNum(TileList.Num());
	USDTileDescriptor* TileToAdd;
	// Ok so until *after* we place a tile that is past the existing borders, we do not turn, that is why
	// This is "previous" neighbors count, and not neighbors count. Please stop getting confused and changing it
	int previousNeighborsCount = 0;
	int currentDirection = 0; // consider 0 degree rotation to be up
	int prevX = 0;
	int prevY = 0;

	// Loop over all locations to be filled in, spiral outward from the center point and add new Tiles that have edges that
	// align with their already placed neighbors
	for (int i = 0; i < TileList.Num(); i++)
	{
		previousNeighborsCount = 0; // reset to 0 so we can recalculate it for every tile

		// Determine if we need to rotate our tile laying direction to keep the spiral going
		if (previousNeighborsCount == 1)
		{
			currentDirection += 90;
			currentDirection = currentDirection % 360;
		}

		// Update the (x, y) of our next tile based on our tile laying direction
		switch (currentDirection)
		{
		case 0:
			prevY += 1;
			break;
		case 90:
			prevX += 1;
			break;
		case 180:
			prevY -= 1;
			break;
		case 270:
			prevX -= 1;
		}

		// put all of our 4 neighbors into a happy little array
		neighborTiles[0] = TileList[IndexFromPoint(prevX, prevY + 1, Params.TileCountX, Params.TileCountY)];
		neighborTiles[1] = TileList[IndexFromPoint(prevX + 1, prevY, Params.TileCountX, Params.TileCountY)];
		neighborTiles[2] = TileList[IndexFromPoint(prevX, prevY - 1, Params.TileCountX, Params.TileCountY)];
		neighborTiles[3] = TileList[IndexFromPoint(prevX - 1, prevY, Params.TileCountX, Params.TileCountY)];
		for (int j = 0; j < 4; j++)
		{
			if (neighborTiles[j]->name != FName("EmptySpace")) // check if we have an already applied neighbor tile or just a placeholder
			{
				previousNeighborsCount++;
			}
			neighborEdges[j] = neighborTiles[j]->GetFacingEdge(j * 90); // we really care about which edge of our neighbor tile faces us, the rest is irrelevant!
		}		

		// Check every tile for this map for validity with our neighborEdges array, the ones that pass will be in a random roll to get chosen
		candidateTiles.Reset();
		for (USDTileDescriptor* Tile : Params.TileSet)
		{
			// We're going to make a new tile so that we can actually control rotation at each point
			TileToAdd = NewObject<USDTileDescriptor>();
			TileToAdd->CopyDescriptorData(candidateTiles.Array()[FMath::RandRange(0, candidateTiles.Num() - 1)]);
			if (TileToAdd->isValidForNeighbors(neighborEdges))
			{
				candidateTiles.Add(TileToAdd);
			}
		}

		TileList[IndexFromPoint(prevX, prevY, Params.TileCountX, Params.TileCountY)] = candidateTiles.Array()[FMath::RandRange(0, candidateTiles.Num() - 1)];
	}

	for (int i = 0; i < TileList.Num(); i++)
	{
// 		TileList[i] = Params.ActorIds[FMath::RandRange(0, Params.ActorIds.Num() - 1)];
		availableIndices[i] = i;
	}

	// Write over locations in tile list. Remove indices from availableIndices to avoid collisions
// 	for (TPair<int, int> KeyFrequencyPair : Params.ExactlyNIds)
// 	{
// 		for (int j = 0; j < KeyFrequencyPair.Value; j++)
// 		{
// 			RandomIndex = FMath::RandRange(0, availableIndices.Num() - 1);
// 			TileList[availableIndices[RandomIndex]] = KeyFrequencyPair.Key;
// 			availableIndices.RemoveAt(RandomIndex);
// 		}
// 	}
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

int ASoulDrive2GameModeBase::IndexFromPoint(int x, int y, int maxX, int maxY)
{
	return x + y * maxY;
}

int32 ASoulDrive2GameModeBase::GetListKeyByIndex(TDoubleLinkedList<int32> &List, int32 Index)
{
	TDoubleLinkedList<int32>::TIterator *Itr = new TDoubleLinkedList<int32>::TIterator(List.GetHead());

	int32 result = -1;
	int32 counter = 0;
	for (int32 value : List)
	{
		if (++counter > Index)
		{
			result = value;
			List.RemoveNode(value);
			break;
		}
	}
	return result;
}

uint8 ASoulDrive2GameModeBase::GetEdgeMapIndex(ProceduralTileEdges inEdge, TArray<FEdgeAlignmentPair> Map)
{
	for (int i = 0; i < Map.Num(); i++)
	{
		if (Map[i].Key == inEdge)
		{
			return i;
		}
	}
	return -1;
}
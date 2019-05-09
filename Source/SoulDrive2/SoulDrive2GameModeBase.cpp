// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include "SDNetPlayerProxy.h"
#include "SDNetPlayerControllerProxy.h"
#include "SoulDrive2GameModeBase.h"
#include "List.h"

#define stringify( name ) # name

//UEnum* pEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT(stringify(ProceduralTileEdges)), true);


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

ASoulDrive2GameModeBase::ASoulDrive2GameModeBase()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> temp(TEXT("DataTable'/Game/SDContent/Levels/InstancedSMLevel/EdgeData_1.EdgeData_1'"));
	EdgeMapData = temp.Object;
	IsLevelActive = false;
}

bool ASoulDrive2GameModeBase::GetIsLevelActive()
{
	return IsLevelActive;
}

void ASoulDrive2GameModeBase::SetLevelActive(bool IsActive)
{
	IsLevelActive = IsActive;
}

/*
* This function will fill TileList with descriptors that can be used to load up a map.  TileList should be passed in as an empty TArray, and if not, it's contents will be ignored and overwritten.
* Params contains all of the necessary data to guide the creation of the map data.
*/
int ASoulDrive2GameModeBase::GenerateMapData(UPARAM(ref) TArray<FTileDescriptor> &TileList, UPARAM(ref) TArray<FIntPair> &ActorLocations, FMapGenerationParams Params)
{

	if (Params.TileCountX < 1 || Params.TileCountY < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot generate map, provided Tile Count is invalid (MaxX = %d, MaxY = %d). Exiting..."), Params.TileCountX, Params.TileCountY);
		return -1;
	}
	int RandomIndex = 0;
	/* 
	** Array of indices not yet overwritten by a "necessary" tile.  Will be filled in with all
	** of the possible indices initially, and those elements will be removed when a needed tile is added on the second pass
	*/
	TArray<int> availableIndices;
	TArray<FTileDescriptor> TileSet;
	TArray<int> openIndices; // Places we can add actors
	TSet<FTileDescriptor *> candidateTiles;
	TArray<ProceduralTileEdges> neighborEdges;
	TArray<FTileDescriptor *> neighborTiles;  // Should always be constructed as Top, Right, Bottom, Left for ordering
	neighborEdges.SetNum(4);
	neighborTiles.SetNum(4);
	TileList.Init(FTileDescriptor(), Params.TileCountX * Params.TileCountY + 1);
	availableIndices.SetNum(TileList.Num());
	FTileDescriptor* TileToAdd;
	
	constructTileSet(TileSet, Params.TileData);

	if (TileSet.Num() < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot generate map, provided TileSet was empty. Exiting..."));
		return -1;
	}

	constructEdgeMap(Params.EdgeMap);
	// Ok so until *after* we place a tile that is past the existing borders, we do not turn, that is why
	// This is "previous" neighbors count, and not neighbors count. Please stop getting confused and changing it
	int previousNeighborsCount = 0;
	int currentDirection = 0; // consider 0 degree rotation to be up
	int index = 0;
	int prevX = Params.TileCountX / 2;
	int prevY = (Params.TileCountY / 2) - 1;

	// Loop over all locations to be filled in, spiral outward from the center point and add new Tiles that have edges that
	// align with their already placed neighbors
	for (int i = 0; i < TileList.Num(); i++)
	{
		// Determine if we need to rotate our tile laying direction to keep the spiral going
		if (previousNeighborsCount == 1)
		{
			currentDirection += 90;
			currentDirection = currentDirection % 360;
		}

		previousNeighborsCount = 0; // reset to 0 so we can recalculate it for every tile

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
		if ((index = IndexFromPoint(prevX, prevY + 1, Params.TileCountX, Params.TileCountY)) < TileList.Num() && index >= 0)
		{
			neighborTiles[0] = &TileList[index];
		}
		else
		{
			neighborTiles[0] = &TileSet[0];
		}
		if ((index = IndexFromPoint(prevX + 1, prevY, Params.TileCountX, Params.TileCountY)) < TileList.Num() && index >= 0)
		{
			neighborTiles[1] = &TileList[index];
		}
		else
		{
			neighborTiles[1] = &TileSet[0];
		}
		if ((index = IndexFromPoint(prevX, prevY - 1, Params.TileCountX, Params.TileCountY)) < TileList.Num() && index >= 0)
		{
			neighborTiles[2] = &TileList[index];
		}
		else
		{
			neighborTiles[2] = &TileSet[0];
		}
		if ((index = IndexFromPoint(prevX - 1, prevY, Params.TileCountX, Params.TileCountY)) < TileList.Num() && index >= 0)
		{
			neighborTiles[3] = &TileList[index];
		}
		else
		{
			neighborTiles[3] = &TileSet[0];
		}
		for (int j = 0; j < 4; j++)
		{
			if (neighborTiles[j]->name != FName("EmptySpace")) // check if we have an already applied neighbor tile or just a placeholder
			{
				previousNeighborsCount++;
			}
			// we really care about which edge of our neighbor tile faces us, the rest is irrelevant!
			neighborEdges[j] = GetFacingEdge(j * 90, neighborTiles[j]);
		}		

		// Check every tile for this map for validity with our neighborEdges array, the ones that pass will be in a random roll to get chosen
		candidateTiles.Reset();
		for (FTileDescriptor Tile : TileSet)
		{
			if (Tile.name != FName("EmptySpace"))
			{
				// We're going to make a new tile so that we can actually control rotation at each point
				TileToAdd = new FTileDescriptor();
				CopyDescriptorData(&Tile, TileToAdd);
				candidateTiles.Append(isValidForNeighbors(neighborEdges, TileToAdd, Params.EdgeMap));
			}
		}

		index = IndexFromPoint(prevX, prevY, Params.TileCountX, Params.TileCountY);
		if (index >= 0 && index < TileList.Num())
		{
			if (candidateTiles.Num() > 0)
			{
				RandomIndex = FMath::RandRange(0, candidateTiles.Num() - 1);
				TileList[index] = *(candidateTiles.Array()[RandomIndex]);
				if (TileList[index].name == FName("Floor"))
				{
					openIndices.Add(index);
				}
			}
			else
			{
				TileList[index] = FTileDescriptor();
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("next tile being placed at invalid location (%d, %d)"), prevX, prevY);
		}
	}

	for (int i = 0; i < TileList.Num(); i++)
	{
// 		TileList[i] = Params.ActorIds[FMath::RandRange(0, Params.ActorIds.Num() - 1)];
		availableIndices[i] = i;
	}

	for (int actor = 0; actor < Params.ActorIds.Num(); actor++)
	{
		if (openIndices.Num() > 0)
		{
			int i = FMath::FRandRange(0, openIndices.Num() - 1);
			ActorLocations.Add(FIntPair(Params.ActorIds[actor], openIndices[i]));
			openIndices.RemoveAt(i);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ran out of open locations to spawn actors"));
		}
	}

// 	for (int32 i : openIndices)
// 	{
// 		ActorLocations.Add(FIntPair(openIndices[i], 0));
// 	}

	return TileList.Num();
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
	return x + y * maxX;
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

ProceduralTileEdges ASoulDrive2GameModeBase::GetFacingEdge(int direction, FTileDescriptor* Desc)
{
	direction -= Desc->rotation;
	direction = (direction + 360) % 360;
	switch (direction)
	{
	case 0:
		return Desc->TopEdge;
		break;
	case 90:
		return Desc->RightEdge;
		break;
	case 180:
		return Desc->BottomEdge;
		break;
	case 270:
		return Desc->LeftEdge;
		break;
	default:
		return Desc->TopEdge;
	}
}

void ASoulDrive2GameModeBase::CopyDescriptorData(FTileDescriptor* CopyFrom, FTileDescriptor* CopyTo)
{
	CopyTo->name = CopyFrom->name;
	CopyTo->LocalId = CopyFrom->LocalId;
	CopyTo->TopEdge = CopyFrom->TopEdge;
	CopyTo->RightEdge = CopyFrom->RightEdge;
	CopyTo->BottomEdge = CopyFrom->BottomEdge;
	CopyTo->LeftEdge = CopyFrom->LeftEdge;
	CopyTo->rotation = CopyFrom->rotation;
}

TArray<FTileDescriptor *> ASoulDrive2GameModeBase::isValidForNeighbors(TArray<ProceduralTileEdges> neighbors, FTileDescriptor* TileToAdd, TArray<FEdgeAlignmentPair> & EdgeMap)
{
	// Loop through every tile in the tileset (nope! handled by caller)
	ProceduralTileEdges Edge;
	uint8 index;
	uint8 acceptedEdgesCount = 0;
	TArray<FTileDescriptor *> returnableTiles; // lets me add multiple different rotations to the candidate pool!
	FTileDescriptor* NextPermutation;

	// For a tile, check it at all rotations
	for (int i = 0; i < 4; i++)
	{
		acceptedEdgesCount = 0;
		NextPermutation = new FTileDescriptor();
		CopyDescriptorData(TileToAdd, NextPermutation);
		NextPermutation->rotation = i * 90;
		// For each rotation, check all of my sides with the candidate neighbors
		for (int j = 0; j < 4; j++)
		{
			Edge = GetFacingEdge(j * 90, NextPermutation);
			index = GetEdgeMapIndex(neighbors[j], EdgeMap);
			if (EdgeMap[index].Value.Contains(Edge))
			{
				acceptedEdgesCount++;
			}
		}
		if (acceptedEdgesCount == 4)
		{
			returnableTiles.Add(NextPermutation);
		}
	}

	return returnableTiles;
}

void ASoulDrive2GameModeBase::constructEdgeMap(TArray<FEdgeAlignmentPair>& EdgeMap)
{
	if (EdgeMapData != nullptr)
	{
		TArray<FEdgeRelationshipTable*> OutAllRows;
		FString Context;
		EdgeMapData->GetAllRows<FEdgeRelationshipTable>(Context, OutAllRows);
		FEdgeAlignmentPair NextPair;
		FName MyName;
		for (FEdgeRelationshipTable* EdgeRow : OutAllRows)
		{
			MyName = FName(*GetEnumValueAsString<ProceduralTileEdges>(stringify(ProceduralTileEdges), NextPair.Key));
			if (EdgeRow->EdgeName != FName(*GetEnumValueAsString<ProceduralTileEdges>(stringify(ProceduralTileEdges), NextPair.Key)))
			{
				if(MyName != FName("None"))
					EdgeMap.Add(NextPair);
				NextPair.Key = GetEnumValueFromString<ProceduralTileEdges>(stringify(ProceduralTileEdges), EdgeRow->EdgeName.ToString());
				NextPair.Value.Reset();
			}
			NextPair.Value.Add(GetEnumValueFromString<ProceduralTileEdges>(stringify(ProceduralTileEdges), EdgeRow->FriendName.ToString()));
		}
		EdgeMap.Add(NextPair);
	}
}

void ASoulDrive2GameModeBase::constructTileSet(TArray<FTileDescriptor> &TileSet, UDataTable* TileData)
{
	FTileDescriptor NextDesc;
	if (TileData != nullptr)
	{
		TArray<FTileEdgesTable*> OutAllRows;
		FString Context;
		TileData->GetAllRows<FTileEdgesTable>(Context, OutAllRows);

		for (FTileEdgesTable* Table : OutAllRows)
		{
			NextDesc.TopEdge = GetEnumValueFromString<ProceduralTileEdges>(stringify(ProceduralTileEdges), Table->Edge1.ToString());
			NextDesc.RightEdge = GetEnumValueFromString<ProceduralTileEdges>(stringify(ProceduralTileEdges), Table->Edge2.ToString());
			NextDesc.BottomEdge = GetEnumValueFromString<ProceduralTileEdges>(stringify(ProceduralTileEdges), Table->Edge3.ToString());
			NextDesc.LeftEdge = GetEnumValueFromString<ProceduralTileEdges>(stringify(ProceduralTileEdges), Table->Edge4.ToString());

			NextDesc.name = Table->TileName;
			NextDesc.LocalId = Table->LocalId;

			TileSet.Add(NextDesc);
		}
	}
}

void ASoulDrive2GameModeBase::PostLogin(APlayerController* NewPlayer)
{
	AGameModeBase::PostLogin(NewPlayer);
	ASDNetPlayerControllerProxy* ProxyController = dynamic_cast<ASDNetPlayerControllerProxy*>(NewPlayer);
	if (ProxyController != nullptr)
	{
		if (!ProxyController->getIsTravelling())
		{
			if (ProxyController->SpawnServerCharacter())
			{
				ProxyController->OnServerCharLoaded();
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to spawn server character due to controller cast failed"));
	}
}

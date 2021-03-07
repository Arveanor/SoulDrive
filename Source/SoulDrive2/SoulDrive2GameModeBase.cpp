// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2GameModeBase.h"
#include "SoulDrive2.h"
//#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include "SDNetPlayerProxy.h"
#include "SDNetPlayerControllerProxy.h"
#include "SDGameInstance.h"
#include "Containers/Array.h"
#include "Math/Box.h"

#define stringify( name ) # name

//UEnum* pEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT(stringify(ProceduralTileEdges)), true);

const TArray<uint16> ASoulDrive2GameModeBase::NUMBER_OF_ROOM_CUTOUT_WEIGHTS = { 1, 3, 5 };

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
	TileSize = 500;
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

int ASoulDrive2GameModeBase::GenerateMapData(UPARAM(ref) TArray<FTileDescriptor> &TileDescriptors)
{
	ensureMsgf(TileSize % 2 == 0, TEXT("TileSize was set to %d. Ensure TileSize is divisible by two before continuing"), TileSize);
	FRandomStream RandomStream;
	RandomStream.GenerateNewSeed(); 
	//RandomStream = FRandomStream(12305);
	//RandomStream = FRandomStream(15453); // a smaller number of hallways
	UE_LOG(LogTemp, Warning, TEXT("Random seed is %d"), RandomStream.GetCurrentSeed());
	MakeQuads(RandomStream, FIntPoint(50, 50), FVector(0, 0, 0));
	TArray<FRoomDescriptor> RoomDescriptors;
	TArray<FHallwayDescriptor> HallwayDescriptors;
	RoomDescriptors = MakeRoomsInQuads(RandomStream);
	HallwayDescriptors = MakeHallways(RandomStream, RoomDescriptors);
	ConnectRoomClusters(HallwayDescriptors, RoomDescriptors);
	BuildTileLocationsList(RoomDescriptors, HallwayDescriptors, TileDescriptors);

	return 0;
}

/*
* This function will fill TileList with descriptors that can be used to load up a map.  TileList should be passed in as an empty TArray, and if not, it's contents will be ignored and overwritten.
* Params contains all of the necessary data to guide the creation of the map data.
*/
// int ASoulDrive2GameModeBase::GenerateMapData(UPARAM(ref) TArray<FTileDescriptor> &TileList, UPARAM(ref) TArray<FIntPair> &ActorLocations, FMapGenerationParams Params)
// {
// 	if (Params.TileCountX < 1 || Params.TileCountY < 1)
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("Cannot generate map, provided Tile Count is invalid (MaxX = %d, MaxY = %d). Exiting..."), Params.TileCountX, Params.TileCountY);
// 		return -1;
// 	}
// 	int RandomIndex = 0;
// 	/* 
// 	** Array of indices not yet overwritten by a "necessary" tile.  Will be filled in with all
// 	** of the possible indices initially, and those elements will be removed when a needed tile is added on the second pass
// 	*/
// 	TArray<int> availableIndices;
// 	TArray<FTileDescriptor> TileSet;
// 	TArray<int> openIndices; // Places we can add actors
// 	TSet<FTileDescriptor *> candidateTiles;
// 	TArray<ProceduralTileEdges> neighborEdges;
// 	TArray<FTileDescriptor *> neighborTiles;  // Should always be constructed as Top, Right, Bottom, Left for ordering
// 	neighborEdges.SetNum(4);
// 	neighborTiles.SetNum(4);
// 	TileList.Init(FTileDescriptor(), Params.TileCountX * Params.TileCountY + 1);
// 	availableIndices.SetNum(TileList.Num());
// 	FTileDescriptor* TileToAdd;
// 	
// 	constructTileSet(TileSet, Params.TileData);
// 
// 	if (TileSet.Num() < 1)
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("Cannot generate map, provided TileSet was empty. Exiting..."));
// 		return -1;
// 	}
// 
// 	constructEdgeMap(Params.EdgeMap);
// 	// Ok so until *after* we place a tile that is past the existing borders, we do not turn, that is why
// 	// This is "previous" neighbors count, and not neighbors count. Please stop getting confused and changing it
// 	int previousNeighborsCount = 0;
// 	int currentDirection = 0; // consider 0 degree rotation to be up
// 	int index = 0;
// 	int prevX = Params.TileCountX / 2;
// 	int prevY = (Params.TileCountY / 2) - 1;
// 
// 	// Loop over all locations to be filled in, spiral outward from the center point and add new Tiles that have edges that
// 	// align with their already placed neighbors
// 	for (int i = 0; i < TileList.Num(); i++)
// 	{
// 		// Determine if we need to rotate our tile laying direction to keep the spiral going
// 		if (previousNeighborsCount == 1)
// 		{
// 			currentDirection += 90;
// 			currentDirection = currentDirection % 360;
// 		}
// 
// 		previousNeighborsCount = 0; // reset to 0 so we can recalculate it for every tile
// 
// 		// Update the (x, y) of our next tile based on our tile laying direction
// 		switch (currentDirection)
// 		{
// 		case 0:
// 			prevY += 1;
// 			break;
// 		case 90:
// 			prevX += 1;
// 			break;
// 		case 180:
// 			prevY -= 1;
// 			break;
// 		case 270:
// 			prevX -= 1;
// 		}
// 
// 		// put all of our 4 neighbors into a happy little array
// 		if ((index = IndexFromPoint(prevX, prevY + 1, Params.TileCountX, Params.TileCountY)) < TileList.Num() && index >= 0)
// 		{
// 			neighborTiles[0] = &TileList[index];
// 		}
// 		else
// 		{
// 			neighborTiles[0] = &TileSet[0];
// 		}
// 		if ((index = IndexFromPoint(prevX + 1, prevY, Params.TileCountX, Params.TileCountY)) < TileList.Num() && index >= 0)
// 		{
// 			neighborTiles[1] = &TileList[index];
// 		}
// 		else
// 		{
// 			neighborTiles[1] = &TileSet[0];
// 		}
// 		if ((index = IndexFromPoint(prevX, prevY - 1, Params.TileCountX, Params.TileCountY)) < TileList.Num() && index >= 0)
// 		{
// 			neighborTiles[2] = &TileList[index];
// 		}
// 		else
// 		{
// 			neighborTiles[2] = &TileSet[0];
// 		}
// 		if ((index = IndexFromPoint(prevX - 1, prevY, Params.TileCountX, Params.TileCountY)) < TileList.Num() && index >= 0)
// 		{
// 			neighborTiles[3] = &TileList[index];
// 		}
// 		else
// 		{
// 			neighborTiles[3] = &TileSet[0];
// 		}
// 		for (int j = 0; j < 4; j++)
// 		{
// 			if (neighborTiles[j]->name != FName("EmptySpace")) // check if we have an already applied neighbor tile or just a placeholder
// 			{
// 				previousNeighborsCount++;
// 			}
// 			// we really care about which edge of our neighbor tile faces us, the rest is irrelevant!
// 			neighborEdges[j] = GetFacingEdge(j * 90, neighborTiles[j]);
// 		}		
// 
// 		// Check every tile for this map for validity with our neighborEdges array, the ones that pass will be in a random roll to get chosen
// 		candidateTiles.Reset();
// 		for (FTileDescriptor Tile : TileSet)
// 		{
// 			if (Tile.name != FName("EmptySpace"))
// 			{
// 				// We're going to make a new tile so that we can actually control rotation at each point
// 				TileToAdd = new FTileDescriptor();
// 				CopyDescriptorData(&Tile, TileToAdd);
// 				candidateTiles.Append(isValidForNeighbors(neighborEdges, TileToAdd, Params.EdgeMap));
// 			}
// 		}
// 
// 		index = IndexFromPoint(prevX, prevY, Params.TileCountX, Params.TileCountY);
// 		if (index >= 0 && index < TileList.Num())
// 		{
// 			if (candidateTiles.Num() > 0)
// 			{
// 				RandomIndex = FMath::RandRange(0, candidateTiles.Num() - 1);
// 				TileList[index] = *(candidateTiles.Array()[RandomIndex]);
// 				if (TileList[index].name == FName("Floor"))
// 				{
// 					openIndices.Add(index);
// 				}
// 			}
// 			else
// 			{
// 				TileList[index] = FTileDescriptor();
// 			}
// 		}
// 		else
// 		{
// 			UE_LOG(LogTemp, Error, TEXT("next tile being placed at invalid location (%d, %d)"), prevX, prevY);
// 		}
// 	}
// 
// 	for (int i = 0; i < TileList.Num(); i++)
// 	{
// // 		TileList[i] = Params.ActorIds[FMath::RandRange(0, Params.ActorIds.Num() - 1)];
// 		availableIndices[i] = i;
// 	}
// 
// 	for (int actor = 0; actor < Params.ActorIds.Num(); actor++)
// 	{
// 		if (openIndices.Num() > 0)
// 		{
// 			int i = FMath::FRandRange(0, openIndices.Num() - 1);
// 			ActorLocations.Add(FIntPair(Params.ActorIds[actor], openIndices[i]));
// 			openIndices.RemoveAt(i);
// 		}
// 		else
// 		{
// 			UE_LOG(LogTemp, Error, TEXT("ran out of open locations to spawn actors"));
// 		}
// 	}
// 
// // 	for (int32 i : openIndices)
// // 	{
// // 		ActorLocations.Add(FIntPair(openIndices[i], 0));
// // 	}
// 
// 	return TileList.Num();
// 	// Write over locations in tile list. Remove indices from availableIndices to avoid collisions
// // 	for (TPair<int, int> KeyFrequencyPair : Params.ExactlyNIds)
// // 	{
// // 		for (int j = 0; j < KeyFrequencyPair.Value; j++)
// // 		{
// // 			RandomIndex = FMath::RandRange(0, availableIndices.Num() - 1);
// // 			TileList[availableIndices[RandomIndex]] = KeyFrequencyPair.Key;
// // 			availableIndices.RemoveAt(RandomIndex);
// // 		}
// // 	}
// }

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
	direction -= Desc->Rotation;
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
	CopyTo->Rotation = CopyFrom->Rotation;
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
		NextPermutation->Rotation = i * 90;
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
				if (MyName != FName("None"))
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

void ASoulDrive2GameModeBase::MakeQuads(FRandomStream RandomStream, FIntPoint MapDimensions, FVector Offset, uint8 RecursionCounter)
{
	// we need to assume a (0, 0) location to build our map against, that overall origin point will be in the bottom left corner
	// x for the vertical slice, y for the horizontal
	FIntPoint Slices = FIntPoint::FIntPoint(RandomStream.FRandRange(0, MapDimensions.X), RandomStream.FRandRange(0, MapDimensions.Y));
	FBox NewQuad;
	FColor DebugColor;
	DrawDebugLine(GetWorld(), Offset + FVector(Slices.X, 0, 0), Offset + FVector(Slices.X, MapDimensions.Y, 0), FColor::Red, true, 990.f, 2.f);
	DrawDebugLine(GetWorld(), Offset + FVector(0, Slices.Y, 0), Offset + FVector(MapDimensions.X, Slices.Y, 0), FColor::Blue, true, 990.f, 2.f);

	switch (RecursionCounter)
	{
	case 0:
		DebugColor = FColor::Emerald;
		break;
	case 1:
		DebugColor = FColor::Red;
		break;
	case 2:
		DebugColor = FColor::Blue;
		break;
	case 3:
		DebugColor = FColor::Silver;
	}

	for (uint8 i = 0; i < 4; i++)
	{
		// 		switch (i)
		// 		{
		// 		case 0:
		// 			DebugColor = FColor::Cyan;
		// 			break;
		// 		case 1:
		// 			DebugColor = FColor::Green;
		// 			break;
		// 		case 2:
		// 			DebugColor = FColor::Magenta;
		// 			break;
		// 		case 3:
		// 			DebugColor = FColor::Yellow;
		// 			break;
		// 		}
		NewQuad = MakeQuad(i, Slices, MapDimensions, FIntPoint(Offset.X, Offset.Y));
		NewQuad = NewQuad.ShiftBy(Offset);
		switch (RecursionCounter)
		{
		case 0:
			DrawDebugBox(GetWorld(), FVector(NewQuad.GetCenter().X, NewQuad.GetCenter().Y, 0), NewQuad.GetSize() / 2, DebugColor, true, 990, 0, 20.f);
			break;
// 		case 1:
// 			DrawDebugBox(GetWorld(), FVector(NewQuad.GetCenter().X, NewQuad.GetCenter().Y, 100), NewQuad.GetSize() / 2, DebugColor, true, 990, 0, 20.f);
// 			break;
// 		case 2:
// 			DrawDebugBox(GetWorld(), FVector(NewQuad.GetCenter().X, NewQuad.GetCenter().Y, 200), NewQuad.GetSize() / 2, DebugColor, true, 990, 0, 20.f);
// 			break;
// 		case 3:
// 			DrawDebugBox(GetWorld(), FVector(NewQuad.GetCenter().X, NewQuad.GetCenter().Y, 300), NewQuad.GetSize() / 2, DebugColor, true, 990, 0, 20.f);
// 			break;
		}

		FVector2D QuadSize = FVector2D(NewQuad.GetSize());
		if (QuadSize.GetMin() < MINIMUM_STEM_QUAD_SIZE || RecursionCounter >= MAXIMUM_QUAD_DEPTH)
		{
			// if we get a super tiny leaf, we are just going to drop it
 			if (QuadSize.GetMin() >= MINIMUM_LEAF_QUAD_SIZE)
 			{
				LeafQuads.Add(NewQuad);
 			}
		}
		else
		{
			StemQuads.Add(NewQuad);
			MakeQuads(RandomStream, FIntPoint(NewQuad.GetSize().X, NewQuad.GetSize().Y), NewQuad.Min, RecursionCounter + 1);
		}
	}

}

FBox ASoulDrive2GameModeBase::MakeQuad(uint8 Id, FIntPoint Slices, FIntPoint MapDimensions, FIntPoint MapOrigin)
{
	// starting top right, our boxes origin will be (MapDimensions[1] - y) / 2 +  y to get our y coordinate, and our x coordinate will be
	// (MapDimensions[0] - x) / 2 + x.  The extent of the bounding box in top right should be (MapDimensions[1] - y) / 2, (MapDimensions[0] - x) / 2

	//MapDimensions - Slices = above
	//Slices = below

	FVector BoxSize, BoxOrigin;
	switch (Id)
	{
	case 0:
		BoxSize = FVector((MapDimensions.X - Slices.X) / 2, (MapDimensions.Y - Slices.Y) / 2, 0);
		BoxOrigin = FVector(BoxSize.X + Slices.X, BoxSize.Y + Slices.Y, 0);
		break;
	case 1:
		BoxSize = FVector(Slices.X / 2, (MapDimensions.Y - Slices.Y) / 2, 0);
		BoxOrigin = FVector(Slices.X - BoxSize.X, BoxSize.Y + Slices.Y, 0);
		break;
	case 2:
		BoxSize = FVector(Slices.X / 2, Slices.Y / 2, 0);
		BoxOrigin = FVector(Slices.X - BoxSize.X, BoxSize.Y, 0);
		break;
	case 3:
		BoxSize = FVector((MapDimensions.X - Slices.X) / 2, Slices.Y / 2, 0);
		BoxOrigin = FVector(BoxSize.X + Slices.X, BoxSize.Y, 0);
		break;
	}


	FBox NewQuad = FBox::BuildAABB(BoxOrigin, BoxSize);
	return NewQuad;
}

TArray<FRoomDescriptor> ASoulDrive2GameModeBase::MakeRoomsInQuads(FRandomStream RandomStream)
{
	TArray<FRoomDescriptor> Results = TArray<FRoomDescriptor>();
	uint16 WeightSum = 0;
	uint8 NumCutouts = 0;
	FRoomDescriptor RoomDescriptor;
	uint8 NumCorners = 4; // We need to be able to adjust this based on the number and placement of cutouts.
	int NextId = 1; // the id counter to give each room a unique id;
	for (int i = 0; i < NUMBER_OF_ROOM_CUTOUT_WEIGHTS.Num(); i++)
	{
		WeightSum += NUMBER_OF_ROOM_CUTOUT_WEIGHTS[i];
	}

	// loop through the list of leaves and create a room size and shape.
	for (FBox Box : LeafQuads)
	{
		if (DEBUG_ROOM_LIMIT <= 0) break;
		DEBUG_ROOM_LIMIT--;
		FIntPoint RoomExtent = FIntPoint(RandomStream.FRandRange(MINIMUM_ROOM_DIMENSIONS, Box.GetSize().X),
			RandomStream.FRandRange(MINIMUM_ROOM_DIMENSIONS, Box.GetSize().Y));

		// Once we know the extent of our room, we should know how much free x and y space we have to adjust the origin of the room.
		// The origin here refers to the bottom left corner, which would be the "Min" vector from an fbox
		FIntPoint RoomOrigin = FIntPoint(RandomStream.FRandRange(0, Box.GetSize().X - RoomExtent.X),
			RandomStream.FRandRange(0, Box.GetSize().Y - RoomExtent.Y)) * TileSize;
		RoomExtent = RoomExtent * TileSize;
		RoomOrigin.X += Box.Min.X * TileSize;
		RoomOrigin.Y += Box.Min.Y * TileSize;
		UE_LOG(LogTemp, Warning, TEXT("FBox: %s"), *Box.ToString());
		UE_LOG(LogTemp, Warning, TEXT("Room Origin at %d, %d"), RoomOrigin.X, RoomOrigin.Y);
		UE_LOG(LogTemp, Warning, TEXT("Room Extent is %d, %d"), RoomExtent.X, RoomExtent.Y);

		// rectangle rooms are boring, let's choose a number of cutouts at random, then choose the location and size of those cutouts at random.
		// may want to limit the amount of area we cutout to a percentage of the boxes whole area, and we certainly don't want any cutouts to be the 
		// same size as the room in either dimension.
		int Random = RandomStream.FRandRange(0, NUMBER_OF_ROOM_CUTOUT_WEIGHTS.Num());
		for (int i = 0; i < NUMBER_OF_ROOM_CUTOUT_WEIGHTS.Num(); i++)
		{
			if (Random < NUMBER_OF_ROOM_CUTOUT_WEIGHTS[i])
			{
				NumCutouts = i;
				break;
			}
			Random -= NUMBER_OF_ROOM_CUTOUT_WEIGHTS[i];
		}

		for (int i = 0; i < NumCutouts; i++)
		{
			// Here we actually determine (in relative space) where to put our cutouts, and how big they should be.
		}

		// Once we know where the cutouts are, we can apply them, and create a list of cutouts.

		// Once we know all of our cutouts, we can (somehow!) construct a list of corners.
		// This will likely turn into a bunch of special cases, but for now we can just create 4 points in order.
		RoomDescriptor = FRoomDescriptor();
		RoomDescriptor.RoomId = NextId++;
		RoomDescriptor.Box = Box;
		RoomDescriptor.OrderedRoomCorners.Add(RoomOrigin + FIntPoint(TileSize, 0));
		RoomDescriptor.OrderedRoomCorners.Add(RoomOrigin + FIntPoint(RoomExtent.X, 0));
		RoomDescriptor.OrderedRoomCorners.Add(RoomOrigin + RoomExtent - FIntPoint(0, TileSize));
		RoomDescriptor.OrderedRoomCorners.Add(RoomOrigin + FIntPoint(TileSize, RoomExtent.Y - TileSize));

		if (RoomDescriptor.RoomId == 3)
		{
			//DrawDebugBox(GetWorld(), FVector(RoomOrigin.X, RoomOrigin.Y, 0), FVector(RoomExtent.X / 2, RoomExtent.Y / 2, 100), FColor::Emerald, true, 990, 0, 20.f);
		}
		if (RoomDescriptor.RoomId == 10)
		{
			//DrawDebugBox(GetWorld(), FVector(RoomOrigin.X, RoomOrigin.Y, 0), FVector(RoomExtent.X / 2, RoomExtent.Y / 2, 100), FColor::Magenta, true, 990, 0, 20.f);
		}
	
		Results.Add(RoomDescriptor);
	}
	return Results;
}


TArray<FHallwayDescriptor> ASoulDrive2GameModeBase::MakeHallways(FRandomStream RandomStream, TArray<FRoomDescriptor> &Rooms)
{
	TArray<FHallwayDescriptor> Results = TArray<FHallwayDescriptor>();

	// TODO: make a quick & dirty hallway algorithm to add n - 1 halls connecting rooms (in no particular order) to their nearest neighbors, then we can look at that and make
	// better decisions

	for (FRoomDescriptor Room : Rooms)
	{
		FHallwayDescriptor NewHallway;

		// find nearest room
		FVector2D RoomCenter = FVector2D(Room.Box.GetCenter().X, Room.Box.GetCenter().Y);
		FRoomDescriptor NearestRoom;
		float Distance = 100000;
		for (FRoomDescriptor RoomMatch : Rooms)
		{
			float DistanceCheck = FVector2D::Distance(RoomCenter, FVector2D(RoomMatch.Box.GetCenter().X, RoomMatch.Box.GetCenter().Y));
			if (DistanceCheck < Distance && RoomMatch.RoomId != Room.RoomId) // given tile sizes, a distance of 1.0 isn't possible, and I'd rather check that than 0.f in case of any weird float math
			{
				Distance = DistanceCheck;
				NearestRoom = RoomMatch;
			}
		}
		//DrawDebugSphere(GetWorld(), Room.Box.GetCenter() * TileSize + FVector(0, 0, 800), 110.f, 4, FColor::Blue, true, -1.f, 12.f);
		//DrawDebugLine(GetWorld(), NearestRoom.Box.GetCenter() * TileSize, Room.Box.GetCenter() * TileSize, FColor::Red, true, 990.f, 12.f);
		UE_LOG(LogTemp, Warning, TEXT("Distance between connected rooms = %f"), Distance);
		UE_LOG(LogTemp, Warning, TEXT("Connected room ids = %d, %d"), Room.RoomId, NearestRoom.RoomId);

		NewHallway.RoomIds.Add(Room.RoomId);
		NewHallway.RoomIds.Add(NearestRoom.RoomId);
		Results.Add(NewHallway);
	}

	return Results;
}

/*
** I need to apply an offset to a doorway based on it's own rotation, but also based on the rotation of the corner it's nearest
*/
FTileDescriptor ASoulDrive2GameModeBase::MakeDoorway(const TArray<FTileDescriptor>& CornerDescriptors, const FRoomDescriptor LocalRoom, const FRoomDescriptor ConnectedRoom)
{
	FTileDescriptor Results;

	float XDiff = LocalRoom.Box.GetCenter().X - ConnectedRoom.Box.GetCenter().X;
	float YDiff = LocalRoom.Box.GetCenter().Y - ConnectedRoom.Box.GetCenter().Y;
	int TargetRotation = -1; // expected rotation of our corner piece to get it's wall to line up correctly with our doorway
	int DoorRotation = 0;
	FName TileName = FName("Doorway");
	FIntPoint DoorLocation;

	TArray <FTileDescriptor> DoorwayCandidates;
	if (FMath::Abs(XDiff) > FMath::Abs(YDiff))
	{
		// start the hallway as horizontal
		if (XDiff > 0)
		{
			// start the hallway on the right side of Room, above a corner
			DoorRotation = 0;
			TargetRotation = 180;
			DoorLocation = FIntPoint(0 - TileSize, 0);
		}
		else
		{
			// start the hallway on the left side of Room
			DoorRotation = 180;
			TargetRotation = 270;
			DoorLocation = FIntPoint(TileSize, TileSize);
		}
	}
	else
	{
		// start the hallway as vertical
		if (YDiff > 0)
		{
			// start the hallway on the bottom of Room
			DoorRotation = 90;
			TargetRotation = 180;
			DoorLocation = FIntPoint(TileSize, 0 - TileSize);
		}
		else
		{
			// start the hallway on the top of Room
			DoorRotation = 270;
			TargetRotation = 90;
			DoorLocation = FIntPoint(0, TileSize);
		}
	}
	for (FTileDescriptor Corner : CornerDescriptors)
	{
		if (Corner.Rotation == TargetRotation)
		{
			DoorLocation += Corner.Location;

			Results = FTileDescriptor(TileName, 4, DoorRotation, DoorLocation);
			break;
		}
	}

	return Results;
}

TArray<FTileDescriptor> ASoulDrive2GameModeBase::PlaceWallTiles(const TArray<FTileDescriptor> &RoomCorners, const TArray<FTileDescriptor> &Doorways)
{
	FIntPoint Curr;
	FIntPoint Next;
	uint8 StraightWallCount;
	TArray<FTileDescriptor> WallDescriptors;
	for (int i = 0; i < RoomCorners.Num(); i++)
	{
		FIntPoint Location;
		int Id = 2;
		int Rotation = 0;
		FName TileName = "Wall_Cave";
		FTileDescriptor NextTile = (RoomCorners.Num() - 1 == i) ? RoomCorners[0] : RoomCorners[i + 1];
		Curr = RoomCorners[i].Location;
		Next = NextTile.Location;
		uint32 CornerToCornerDist = FMath::Max(FMath::Abs(Curr.X - Next.X), FMath::Abs(Curr.Y - Next.Y));
		// subtract tilesize here because two adjacent concave corners won't need any adjoining walls.
		StraightWallCount = CornerToCornerDist / TileSize;
		if (NextTile.name == FName("Corner_Convex"))
		{
			StraightWallCount++;
		}

		if (RoomCorners[i].name == FName("Corner_Convex"))
		{
			StraightWallCount++;
			if (RoomCorners[i].Rotation == 0)
			{
				// this should put our corner piece in the bottom left corner of the tile, with our tiles having their walls on the right, and being offset one
				// tile width to the left of our current position
				for (int j = 0; j < StraightWallCount; j++)
				{
					Rotation = 270;
					Location = FIntPoint(Curr.X - TileSize, Curr.Y + (TileSize * j));
					if (!FindDoorConflict(Doorways, Location))
					{
						WallDescriptors.Emplace(TileName, Id, Rotation, Location);
					}
				}

			}
			else if (RoomCorners[i].Rotation == 90)
			{
				// walls here are up 1 tile and leading off to the right.
				for (int j = 0; j < StraightWallCount; j++)
				{
					Rotation = 0;
					Location = FIntPoint(Curr.X + (TileSize * j), Curr.Y + TileSize);
					if (!FindDoorConflict(Doorways, Location))
					{
						WallDescriptors.Emplace(TileName, Id, Rotation, Location);

					}
				}
			}
			else if (RoomCorners[i].Rotation == 180)
			{
				// walls here are up 1 tile and leading off to the left.
				for (int j = 0; j < StraightWallCount; j++)
				{
					Rotation = 0;
					Location = FIntPoint(Curr.X - (TileSize * j), Curr.Y + TileSize);
					if(!FindDoorConflict(Doorways, Location))
						WallDescriptors.Emplace(TileName, Id, Rotation, Location);
				}
			}
			else if (RoomCorners[i].Rotation == 270)
			{
				// walls here are going up and are 1 to the right
				for (int j = 0; j < StraightWallCount; j++)
				{
					Rotation = 90;
					Location = FIntPoint(Curr.X + TileSize, Curr.Y + (TileSize * j));
					if(!FindDoorConflict(Doorways, Location))
						WallDescriptors.Emplace(TileName, Id, Rotation, Location);
				}
			}
		}
		else if (RoomCorners[i].name == FName("Corner_Concave"))
		{
			if (RoomCorners[i].Rotation == 0)
			{
				for (int j = 0; j < StraightWallCount; j++)
				{
					Rotation = 270;
					Location = FIntPoint(Curr.X - (TileSize * (j + 1)), Curr.Y + TileSize);
					if(!FindDoorConflict(Doorways, Location)) 
						WallDescriptors.Emplace(TileName, Id, Rotation, Location);
				}
			}
			else if (RoomCorners[i].Rotation == 90)
			{
				for (int j = 0; j < StraightWallCount; j++)
				{
					Rotation = 0;
					Location = FIntPoint(Curr.X - TileSize, Curr.Y - (TileSize * (j + 1)));
					if(!FindDoorConflict(Doorways, Location))
						WallDescriptors.Emplace(TileName, Id, Rotation, Location);
				}
			}
			else if (RoomCorners[i].Rotation == 180)
			{
				for (int j = 0; j < StraightWallCount; j++)
				{
					Rotation = 90;
					Location = FIntPoint(Curr.X + (TileSize * (j + 1)), Curr.Y - TileSize);
					if(!FindDoorConflict(Doorways, Location))
						WallDescriptors.Emplace(TileName, Id, Rotation, Location);
				}
			}
			else if (RoomCorners[i].Rotation == 270)
			{
				for (int j = 0; j < StraightWallCount; j++)
				{
					Rotation = 180;
					Location = FIntPoint(Curr.X + TileSize, Curr.Y + (TileSize * (j + 1)));
					if(!FindDoorConflict(Doorways, Location))
						WallDescriptors.Emplace(TileName, Id, Rotation, Location);
				}
			}
		}
	}
	return WallDescriptors;
}

void ASoulDrive2GameModeBase::BuildTileLocationsList(TArray<FRoomDescriptor> Rooms, TArray<FHallwayDescriptor> Hallways, TArray<FTileDescriptor> &TileDescriptors)
{
	// our blueprint will handle spawning all these damn things, we just need to transform the information of the rooms and hallways into tile name, location, and rotation
	// for the bp to handle the spawning. We shouldn't need to use our old trick of expanding array indices into location data since we already know the locations of each
	// of our rooms here.

	for (FRoomDescriptor Room : Rooms)
	{
		FIntPoint Prev;
		FIntPoint Curr;
		FIntPoint AdjustedLocation;
		FIntPoint Next;
		TArray<FTileDescriptor> WallDescriptors; // Necessary so that I can loop through corners without adding walls to the full list until I'm done.
		TArray<FTileDescriptor> RoomCorners; // Similar to above, store all of the rooms corner tiles in their own array temporarily so we can iterate properly.
		TArray<FTileDescriptor> Doorways;

		// for loop to create tiles of each room corner
		for (int i = 0; i < Room.OrderedRoomCorners.Num(); i++)
		{
			Curr = Room.OrderedRoomCorners[i];
			Next = (Room.OrderedRoomCorners.Num() - 1 == i) ? Room.OrderedRoomCorners[0] : Room.OrderedRoomCorners[i + 1];
			Prev = (i == 0) ? Room.OrderedRoomCorners[Room.OrderedRoomCorners.Num() - 1] : Room.OrderedRoomCorners[i - 1];
			AdjustedLocation.X = Curr.X;
			AdjustedLocation.Y = Curr.Y;
			// this loop can only give us inward and outward corners, but we can assume straight wall border pieces between each point, and we'll need
			// to reference tilesize to place the internal (non border) tiles.
			
			// unreal has a flipped x axis, so if prev.x is smaller than curr.x than prev.x is to the right of curr.x
			// prev is to the left of curr
			if (Curr.X < Prev.X)
			{
				if (Curr.Y < Next.Y)
				{
					// inward corner, this is actually comprised of 3 different tiles, only one of which has the corner, the other two are walls on a single side.
					// the actual "corner" tile will have it's corner in the bottom right here.
					int Id = 1;
					int rotation = 0;
					FName TileName = FName("Corner_Convex");
					RoomCorners.Emplace(TileName, Id, rotation, AdjustedLocation);
				}
				else
				{
					// outward corner, walls on top and right side of tile
					int Id = 1;
					int rotation = 90;
					FName TileName = FName("Corner_Concave");
					AdjustedLocation.X += TileSize;
					RoomCorners.Emplace(TileName, Id, rotation, AdjustedLocation);
				}
			}
			else if (Curr.X > Prev.X) // prev is to the right of curr
			{
				if (Curr.Y < Next.Y)
				{
					// outward corner, walls are on bottom and left side
					int Id = 1;
					int rotation = 270;
					FName TileName = FName("Corner_Concave");
					AdjustedLocation.Y += TileSize;
					RoomCorners.Emplace(TileName, Id, rotation, AdjustedLocation);
				}
				else
				{
					// inward corner, requires 3 tiles, the corner tile itself will have it's corner on top left.
					int Id = 1;
					int rotation = 0;
					FName TileName = FName("Corner_Convex");
					RoomCorners.Emplace(TileName, Id, rotation, Curr);
				}
			}
			else if (Curr.Y > Prev.Y)
			{
				if (Curr.X > Next.X) // next is to the right of curr
				{
					// outward corner, walls are on the top and left.
					int Id = 1;
					int rotation = 0;
					FName TileName = FName("Corner_Concave");
					RoomCorners.Emplace(TileName, Id, rotation, AdjustedLocation);
				}
				else
				{
					// inward corner, requires 3 tiles, the corner tile will have it's corner on bottom left.
					int Id = 1;
					int rotation = 0;
					FName TileName = FName("Corner_Convex");
					RoomCorners.Emplace(TileName, Id, rotation, Curr);
				}
			}
			else
			{
				if (Curr.X > Next.X) // next is to the right of curr
				{
					// inward corner, requires 3 tiles, the corner tile will have it's corner on top right here.
					int Id = 1;
					int rotation = 0;
					FName TileName = FName("Corner_Convex");
					RoomCorners.Emplace(TileName, Id, rotation, AdjustedLocation);
				}
				else
				{
					// outward corner, walls are on the bottom and right
					int Id = 1;
					int rotation = 180;
					FName TileName = FName("Corner_Concave");
					AdjustedLocation.X += TileSize;
					AdjustedLocation.Y += TileSize;
					RoomCorners.Emplace(TileName, Id, rotation, AdjustedLocation);
				}
			}
		} // end for loop to tile-ify corners
		Doorways.Empty();
		// here we can place hall entrance tiles based on a hallway descriptor of room id <--> room id
		for (FHallwayDescriptor &Hall : Hallways)
		{
			if (Room.RoomId == Hall.RoomIds[0])
			{
				for (FRoomDescriptor ConnectedRoom : Rooms)
				{
					if (ConnectedRoom.RoomId == Hall.RoomIds[1])
					{
						FTileDescriptor Door = MakeDoorway(RoomCorners, Room, ConnectedRoom);
						Doorways.Add(Door);
						Hall.DoorLocations.Add(Door);
					}
				}
			}
			if (Room.RoomId == Hall.RoomIds[1])
			{
				for (FRoomDescriptor ConnectedRoom : Rooms)
				{
					if (ConnectedRoom.RoomId == Hall.RoomIds[0])
					{
						FTileDescriptor Door = MakeDoorway(RoomCorners, Room, ConnectedRoom);
						Doorways.Add(Door);
						Hall.DoorLocations.Add(Door);
					}
				}
			}
		}

		// UE_LOG(LogTemp, Warning, TEXT("Doorway Pair Failures = %d"), failed);
		// place straight cave walls around room
		WallDescriptors = PlaceWallTiles(RoomCorners, Doorways);

		// place floor tiles loop
		WallDescriptors.Append(Doorways);
		TArray<FTileDescriptor> WallsAndDoors = WallDescriptors;
		WallsAndDoors.Append(Doorways);
		PlaceFloorTiles(TileDescriptors, WallsAndDoors);

		TileDescriptors.Append(Doorways);
		TileDescriptors.Append(RoomCorners);
		TileDescriptors.Append(WallDescriptors);
	}
	for (FHallwayDescriptor Hallway : Hallways)
	{
		int failed = 0;
		if (Hallway.DoorLocations.Num() > 1)
		{
			DrawDebugLine(GetWorld(), FVector(Hallway.DoorLocations[0].Location.X, Hallway.DoorLocations[0].Location.Y, 0),
				FVector(Hallway.DoorLocations[1].Location.X, Hallway.DoorLocations[1].Location.Y, 0), FColor::Red, true, 990.f, 12.f);
			UE_LOG(LogTemp, Warning, TEXT("Doorway Pair: (%d, %d, %d, %d) | (%d, %d, %d, %d)"), Hallway.DoorLocations[0].Location.X, Hallway.DoorLocations[0].Location.Y,
				Hallway.DoorLocations[0].Rotation, Hallway.RoomIds[0], Hallway.DoorLocations[1].Location.X, Hallway.DoorLocations[1].Location.Y, 
				Hallway.DoorLocations[1].Rotation, Hallway.RoomIds[1]);
		}
		else
			failed++;
		PlaceHallTiles(TileDescriptors, Hallway);
	}
}

void ASoulDrive2GameModeBase::PlaceFloorTiles(TArray<FTileDescriptor> &TileDescriptors, TArray<FTileDescriptor> WallDescriptors)
{
	for (FTileDescriptor Wall : WallDescriptors)
	{
		if (Wall.Rotation == 180)
		{
			for (FTileDescriptor PairedWall : WallDescriptors)
			{
				if (PairedWall.Rotation == 0 && Wall.Location.Y - TileSize == PairedWall.Location.Y)
				{
					uint32 WallToWallDist = FMath::Abs(Wall.Location.X - PairedWall.Location.X);
					int FloorCount = (WallToWallDist / TileSize) - 2;
					for (int j = 0; j < FloorCount; j++)
					{
						FName TileName = FName("Floor_Cave");
						FIntPoint Location = FIntPoint(Wall.Location.X - TileSize * j - TileSize * 2, Wall.Location.Y - TileSize);
						TileDescriptors.Emplace(TileName, 3, 0, Location);
					}
				}
			}
		}
	}
}

void ASoulDrive2GameModeBase::PlaceHallTiles(TArray<FTileDescriptor> &TileDescriptors, FHallwayDescriptor Hallway)
{
	TArray<FIntPoint> TileCenters;
	CreateHallwayRoute(Hallway.DoorLocations[0], Hallway.DoorLocations[1], TileCenters);
	TileDescriptors.Append(CreateHallTilesFromRoute(TileCenters));
}

bool ASoulDrive2GameModeBase::FindDoorConflict(const TArray<FTileDescriptor> &Doorways, const FIntPoint Location)
{
	for (FTileDescriptor Door : Doorways)
	{

		if (Location == Door.Location)
		{
			return true;
		}
	}
	return false;
}

void ASoulDrive2GameModeBase::CalculateClusterDistances(TArray<FClusterPair>& DistanceArray, TArray <FRoomCluster>& Clusters)
{
	// Make all pairs of Clusters
	// Calculate the distance of each pair
	// Sort the Array
	// And that's all we have to do in thsi helper
	DistanceArray.Empty();
	for (int i = 0; i < Clusters.Num(); i++)
	{
		for (int j = i + 1; j < Clusters.Num(); j++)
		{
			FClusterPair ClusterPair;
			FIntPoint DistanceVector = Clusters[i].Center - Clusters[j].Center;
			ClusterPair.Start = Clusters[i];
			ClusterPair.End = Clusters[j];
			ClusterPair.Distance = DistanceVector.Size();
			DistanceArray.Add(ClusterPair);
		}
	}

	DistanceArray.Sort();
}

void ASoulDrive2GameModeBase::CombineClusters(TArray<FClusterPair>& DistanceArray, TArray <FRoomCluster>& Clusters, TArray<FHallwayDescriptor> &Hallways)
{
	//build a hallway linking the clusters in the first cluster pair
	// this will mean finding the closest pair of rooms in each cluster
	int StartId, EndId;
	FHallwayDescriptor NewHallway;

	while (DistanceArray.Num() > 0)
	{
		NewHallway.RoomIds.Empty();
		int32 MinBuffer = MAX_int32;

		for (FRoomDescriptor room : DistanceArray[0].Start.Rooms)
		{
			for (FRoomDescriptor targetRoom : DistanceArray[0].End.Rooms)
			{
				FIntPoint DistanceVector = FIntPoint(room.Box.GetCenter().X, room.Box.GetCenter().Y)
					- FIntPoint(targetRoom.Box.GetCenter().X, targetRoom.Box.GetCenter().Y);
				if (DistanceVector.Size() < MinBuffer)
				{
					MinBuffer = DistanceVector.Size();
					StartId = room.RoomId;
					EndId = targetRoom.RoomId;
				}
			}
		}
		NewHallway.RoomIds.Add(StartId);
		NewHallway.RoomIds.Add(EndId);
		Hallways.Add(NewHallway);

		Clusters.Remove(DistanceArray[0].Start);
		Clusters.Remove(DistanceArray[0].End);
		DistanceArray[0].Start.Rooms.Append(DistanceArray[0].End.Rooms);
		FindClusterCenter(DistanceArray[0].Start);
		Clusters.Add(DistanceArray[0].Start);
		CalculateClusterDistances(DistanceArray, Clusters);
	}
}

// It is possible that making our room clusters could be handled every time we form an initial hallway connection.
void ASoulDrive2GameModeBase::ConnectRoomClusters(TArray<FHallwayDescriptor> &Hallways, TArray<FRoomDescriptor> &Rooms)
{
	int IdCounter = 1;
	TArray<FRoomCluster> Clusters;
	TArray<FHallwayDescriptor> ProcessingHallways = Hallways;
	for (FHallwayDescriptor Hallway : Hallways)
	{
		FRoomCluster Cluster;
		if (!IsHallwayInAnyCluster(Hallway, Clusters)) // if neither hallway room is present in Clusters
		{	
			CreateRoomCluster(ProcessingHallways, Hallway, Cluster, Rooms);
			Cluster.ClusterId = IdCounter++;
			FindClusterCenter(Cluster);
			Clusters.Add(Cluster);
		}
	}

	// Find the distance between all possible pairs of clusters.
	TArray<FClusterPair> Distances;
	CalculateClusterDistances(Distances, Clusters);
	CombineClusters(Distances, Clusters, Hallways);
	// we want to add those two clusters together, recompute it's center
	// and recompute all shortest paths.
	// we can do this until there are no remaining clusters ^.^

}

void ASoulDrive2GameModeBase::CreateRoomCluster(TArray<FHallwayDescriptor>& Hallways, const FHallwayDescriptor &Hallway, FRoomCluster &Cluster, const TArray<FRoomDescriptor>& Rooms, int debugRecursion)
{
	FHallwayDescriptor PassthroughCopy; // I need to pass my hallway pointer as a regular object so that it can be removed.

	/*
	* Recursion is creating a stack overflow or something of that ilk, please investigate
	* Please don't be a fucking dolt and use the below api to replace the foreach loop on Hallways below thanks future me
	* https://docs.unrealengine.com/en-US/API/Runtime/Core/Containers/TArray/FindByPredicate/2/index.html
	*/
	Hallways.Remove(Hallway);

	for (FRoomDescriptor Room : Rooms)
	{
		if (Hallway.RoomIds.Contains(Room.RoomId))
		{
			Cluster.Rooms.Add(Room);
		}
	}

	FHallwayDescriptor* Hall = nullptr;
	do 
	{
		Hall = Hallways.FindByPredicate([Hallway](const FHallwayDescriptor& InItem)
		{
			return InItem.RoomIds.Contains(Hallway.RoomIds[0]) || InItem.RoomIds.Contains(Hallway.RoomIds[1]);
		});
		if (Hall != nullptr)
		{
			PassthroughCopy.RoomIds = Hall->RoomIds;
			CreateRoomCluster(Hallways, PassthroughCopy, Cluster, Rooms, 1);
		}

	} while (Hall != nullptr);

// 
// 	for (FHallwayDescriptor Hall : Hallways)
// 	{
// 		if (Hall.RoomIds.Contains(Hallway.RoomIds[0]) || Hall.RoomIds.Contains(Hallway.RoomIds[1]))
// 		{
// 			counter++;
// 			CreateRoomCluster(Hallways, Hall, Cluster, Rooms, debugRecursion + 1);
// 		}
// 	}
// 	UE_LOG(LogTemp, Warning, TEXT("this many %d"), counter);
}

bool ASoulDrive2GameModeBase::IsHallwayInAnyCluster(const FHallwayDescriptor &Hallway, const TArray<FRoomCluster> &Clusters)
{
	for (FRoomCluster Cluster : Clusters)
	{
		for (FRoomDescriptor room : Cluster.Rooms)
		{
			if (Hallway.RoomIds.Contains(room.RoomId))
				return true;
		}
	}
	return false;
}

FIntPoint ASoulDrive2GameModeBase::FindClusterCenter(FRoomCluster& Cluster)
{
	FIntPoint Center = FIntPoint(0, 0);
	for (FRoomDescriptor& room : Cluster.Rooms)
	{
		Center.X += room.Box.GetCenter().X;
		Center.Y += room.Box.GetCenter().Y;
	}
	Center.X = Center.X / Cluster.Rooms.Num();
	Center.Y = Center.Y / Cluster.Rooms.Num();

	Cluster.Center = Center;
	return Cluster.Center;
}

/*
** Making some bold assumptions here that every map tile will have its origin on it's bottom right corner when z rotation = 0.
*/
FIntPoint ASoulDrive2GameModeBase::FindTileOriginFromCenter(FIntPoint Center, int rotation)
{
	FIntPoint TileCorner;
	switch (rotation)
	{
	case 0:
		TileCorner = Center - FIntPoint(TileSize / 2, TileSize / 2);
		break;
	case 90:
		TileCorner = Center + FIntPoint(TileSize / 2, -1 * TileSize / 2);
		break;
	case 180:
		TileCorner = Center + FIntPoint(TileSize / 2, TileSize / 2);
		break;
	case 270:
		TileCorner = Center + FIntPoint(-1 * TileSize / 2, TileSize / 2);
		break;
	}
	return TileCorner;
}

FIntPoint ASoulDrive2GameModeBase::FindTileCenterFromOrigin(const FTileDescriptor Tile)
{
	FIntPoint TileCenter;
	switch (Tile.Rotation)
	{
	case 0:
		TileCenter = Tile.Location + FIntPoint(TileSize / 2, TileSize / 2);
		break;
	case 90:
		TileCenter = Tile.Location + FIntPoint(-1 * TileSize / 2, TileSize / 2);
		break;
	case 180:
		TileCenter = Tile.Location - FIntPoint(TileSize / 2, TileSize / 2);
		break;
	case 270:
		TileCenter = Tile.Location + FIntPoint(TileSize / 2, -1 * TileSize / 2);
		break;
	}
	return TileCenter;
}

void ASoulDrive2GameModeBase::CreateHallwayRoute(const FTileDescriptor& Start, const FTileDescriptor& End, TArray<FIntPoint>& Solution)
{
	FIntPoint StartCenter = FindTileCenterFromOrigin(Start);
	FIntPoint EndCenter = FindTileCenterFromOrigin(End);
	FIntPoint PenultimateCenter;
	FIntPoint Cursor;
	TArray<int> Scores;
	int32 ScoresIndex = 0;

	Solution.Add(StartCenter);

	switch (Start.Rotation)
	{
	case 0:
		StartCenter -= FIntPoint(TileSize, 0);
		break;
	case 90:
		StartCenter -= FIntPoint(0, TileSize);
		break;
	case 180:
		StartCenter += FIntPoint(TileSize, 0);
		break;
	case 270:
		StartCenter += FIntPoint(0, TileSize);
		break;
	}
	Cursor = StartCenter;

	switch (End.Rotation)
	{
	case 0:
		PenultimateCenter = EndCenter - FIntPoint(TileSize, 0);
		break;
	case 90:
		PenultimateCenter = EndCenter - FIntPoint(0, TileSize);
		break;
	case 180:
		PenultimateCenter = EndCenter + FIntPoint(TileSize, 0);
		break;
	case 270:
		PenultimateCenter = EndCenter + FIntPoint(0, TileSize);
		break;
	}

	Solution.Add(StartCenter);
	int j = 0;

	while (Cursor != PenultimateCenter)
	{
		Scores.Empty();
		// calculate values for each direction
		Scores.Add(ScoreHallwayStep(Cursor, PenultimateCenter, Cursor + FIntPoint(TileSize, 0)));
		Scores.Add(ScoreHallwayStep(Cursor, PenultimateCenter, Cursor + FIntPoint(-1 * TileSize, 0)));
		Scores.Add(ScoreHallwayStep(Cursor, PenultimateCenter, Cursor + FIntPoint(0, TileSize)));
		Scores.Add(ScoreHallwayStep(Cursor, PenultimateCenter, Cursor + FIntPoint(0, -1 * TileSize)));

		FMath::Min(Scores, &ScoresIndex);

		switch (ScoresIndex)
		{
		case 0:
			Cursor += FIntPoint(TileSize, 0);
			break;
		case 1:
			Cursor += FIntPoint(-1 * TileSize, 0);
			break;
		case 2:
			Cursor += FIntPoint(0, TileSize);
			break;
		case 3:
			Cursor += FIntPoint(0, -1 * TileSize);
			break;
		}

		Solution.Add(Cursor);
		j++;
	} 
	Solution.Add(EndCenter);

}

int ASoulDrive2GameModeBase::ScoreHallwayStep(FIntPoint Cursor, FIntPoint Target, FIntPoint Candidate)
{
	FIntPoint DistanceG = Candidate - Cursor;
	FIntPoint DistanceH = Candidate - Target;
	int CandidateScore = DistanceH.Size(); // +DistanceH.Size();

	return CandidateScore;
}

TArray<FTileDescriptor> ASoulDrive2GameModeBase::CreateHallTilesFromRoute(const TArray<FIntPoint>& Route)
{
	TArray<FTileDescriptor> Tiles;
	int Rotation = 0;
	FIntPoint Location;
	FName TileName;

	//make a rolling 3 tile buffer
	//determine tile type by neighbors
	//determine rotation by neighbors
	//looping over 1 to Num() - 1 since Route inludes the doorway tiles.
	for (int i = 1; i < Route.Num() - 1; i++)
	{
		int xdiff = Route[i - 1].X - Route[i + 1].X;
		int ydiff = Route[i - 1].Y - Route[i + 1].Y;
		if (xdiff != 0 && ydiff != 0)
		{
			//X and Y changed, Route[i] must be a corner
			TileName = FName("Hallway_Cave_Curve");
			if (Route[i - 1].X == Route[i].X)
			{
				if (Route[i - 1].Y > Route[i].Y)
				{
					if (Route[i + 1].X > Route[i].X)
					{
						Rotation = 180;
					}
					else
					{
						Rotation = 270;
					}
				}
				else
				{
					if (Route[i + 1].X > Route[i].X)
					{
						Rotation = 90;
					}
					else
					{
						Rotation = 0;
					}
				}
			}
			else if (Route[i - 1].X > Route[i].X)
			{
				if (Route[i + 1].Y > Route[i].Y)
				{
					Rotation = 180;
				}
				else
				{
					Rotation = 90;
				}
			}
			else
			{
				if (Route[i + 1].Y > Route[i].Y)
				{
					Rotation = 270;
				}
				else
				{
					Rotation = 0;
				}
			}
		}
		else
		{
			//Here we are just a straight hallway
			TileName = FName("Hallway_Cave_Straight");
			if (xdiff == 0)
			{
				Rotation = 0;
			}
			else
			{
				Rotation = 90;
			}
		}
		Location = FindTileOriginFromCenter(Route[i], Rotation);
		Tiles.Emplace(TileName, 5, Rotation, Location);
	}

	return Tiles;
}

void ASoulDrive2GameModeBase::PostLogin(APlayerController* NewPlayer)
{
	AGameModeBase::PostLogin(NewPlayer);
}

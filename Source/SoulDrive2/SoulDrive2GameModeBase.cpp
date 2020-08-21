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
	TileSize = 400;
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
	FRandomStream RandomStream = FRandomStream(2343);
	MakeQuads(RandomStream, FIntPoint(50, 50), FVector(8000, 8000, 0));
	TArray<FRoomDescriptor> RoomDescriptors;
	TArray<FHallwayDescriptor> HallwayDescriptors;
	RoomDescriptors = MakeRoomsInQuads(RandomStream);
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
	// 	DrawDebugLine(GetWorld(), Offset + FVector(Slices.X, 0, 0), Offset + FVector(Slices.X, MapDimensions.Y, 0), FColor::Red, true, 990.f, 2.f);
	// 	DrawDebugLine(GetWorld(), Offset + FVector(0, Slices.Y, 0), Offset + FVector(MapDimensions.X, Slices.Y, 0), FColor::Blue, true, 990.f, 2.f);

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
		// 		switch (RecursionCounter)
		// 		{
		// 		case 0:
		// 			DrawDebugBox(GetWorld(), FVector(NewQuad.GetCenter().X, NewQuad.GetCenter().Y, 0), NewQuad.GetSize() / 2, DebugColor, true, 990, 0, 20.f);
		// 			break;
		// 		case 1:
		// 			DrawDebugBox(GetWorld(), FVector(NewQuad.GetCenter().X, NewQuad.GetCenter().Y, 100), NewQuad.GetSize() / 2, DebugColor, true, 990, 0, 20.f);
		// 			break;
		// 		case 2:
		// 			DrawDebugBox(GetWorld(), FVector(NewQuad.GetCenter().X, NewQuad.GetCenter().Y, 200), NewQuad.GetSize() / 2, DebugColor, true, 990, 0, 20.f);
		// 			break;
		// 		case 3:
		// 			DrawDebugBox(GetWorld(), FVector(NewQuad.GetCenter().X, NewQuad.GetCenter().Y, 300), NewQuad.GetSize() / 2, DebugColor, true, 990, 0, 20.f);
		// 			break;
		// 		}

		if (/*NewQuad.GetSize().GetMax() < MINIMUM_STEM_QUAD_SIZE ||*/ RecursionCounter >= MAXIMUM_QUAD_DEPTH)
		{
			// if we get a super tiny leaf, we are just going to drop it
// 			if (NewQuad.GetSize().GetMin() > MINIMUM_LEAF_QUAD_SIZE)
// 			{
			LeafQuads.Add(NewQuad);
			// 			}
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
	for (int i = 0; i < NUMBER_OF_ROOM_CUTOUT_WEIGHTS.Num(); i++)
	{
		WeightSum += NUMBER_OF_ROOM_CUTOUT_WEIGHTS[i];
	}

	// loop through the list of leaves and create a room size and shape.
	for (FBox Box : LeafQuads)
	{
		FIntPoint RoomExtent = FIntPoint(RandomStream.FRandRange(MINIMUM_ROOM_DIMENSIONS, Box.GetSize().X),
			RandomStream.FRandRange(MINIMUM_ROOM_DIMENSIONS, Box.GetSize().Y)) * TileSize;

		// Once we know the extent of our room, we should know how much free x and y space we have to adjust the origin of the room.
		// The origin here refers to the bottom left corner, which would be the "Min" vector from an fbox
		FIntPoint RoomOrigin = FIntPoint(RandomStream.FRandRange(0, Box.GetSize().X * TileSize - RoomExtent.X),
			RandomStream.FRandRange(0, Box.GetSize().Y * TileSize - RoomExtent.Y));

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
		RoomDescriptor.OrderedRoomCorners.Add(RoomOrigin);
		RoomDescriptor.OrderedRoomCorners.Add(RoomOrigin + FIntPoint(0, RoomExtent.Y));
		RoomDescriptor.OrderedRoomCorners.Add(RoomOrigin + RoomExtent);
		RoomDescriptor.OrderedRoomCorners.Add(RoomOrigin + FIntPoint(RoomExtent.X, 0));
		Results.Add(RoomDescriptor);
	}
	return Results;
}

TArray<FHallwayDescriptor> ASoulDrive2GameModeBase::MakeHallways(FRandomStream RandomStream)
{
	TArray<FHallwayDescriptor> Results = TArray<FHallwayDescriptor>();

	// we probably want to accept the list of room descriptors here, maybe enhancing those with min/max
	// if we know the min/max extents of a room, we can use that to find nearby neighbors, but we can also 
	// probably do some simple work to ensure that rooms are in some type of order, and use that as our basis for finding neighbors
	// also not sure about how much I want non neighbors connecting, or how often a room should have more than 2 halls, or if halls should intersect.

	// TODO: make a quick & dirty hallway algorithm to add n - 1 halls connecting rooms (in no particular order) to their nearest neighbors, then we can look at that and make
	// better decisions

	return Results;
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
		FIntPoint Next;
		uint8 StraightWallCount;
		TArray<FTileDescriptor> WallDescriptors; // Necessary so that I can loop through corners without adding walls to the full list until I'm done.
		TArray<FTileDescriptor> RoomCorners; // Similar to above, store all of the rooms corner tiles in their own array temporarily so we can iterate properly.

		for (int i = 1; i < Room.OrderedRoomCorners.Num(); i++)
		{
			Curr = Room.OrderedRoomCorners[i];
			Next = (Room.OrderedRoomCorners.Num() - 1 == i) ? Room.OrderedRoomCorners[0] : Room.OrderedRoomCorners[i + 1];
			Prev = (i == 0) ? Room.OrderedRoomCorners[Room.OrderedRoomCorners.Num() - 1] : Room.OrderedRoomCorners[i - 1];
			// this loop can only give us inward and outward corners, but we can assume straight wall border pieces between each point, and we'll need
			// to reference tilesize to place the internal (non border) tiles.
			// since we are moving clockwise around the room, we know that if our current corners x is smaller than previous x, a decrease in y moving to next y
			// means that this is an inward corner, if y increases, it's an outward corner.
			if (Curr.X < Prev.X)
			{
				if (Curr.Y < Next.Y)
				{
					// outward corner, walls on bottom and left side of tile
					int Id = 1;
					int rotation = 0;
					FName TileName = FName("Corner_Concave");
					RoomCorners.Emplace(TileName, Id, rotation, Curr);
				}
				else
				{
					// inward corner, this is actually comprised of 3 different tiles, only one of which has the corner, the other two are walls on a single side.
					// the actual "corner" tile will have it's corner in the bottom right here.
					int Id = 1;
					int rotation = 0;
					FName TileName = FName("Corner_Concave");
					RoomCorners.Emplace(TileName, Id, rotation, Curr);
				}
			}
			else if (Curr.X > Prev.X)
			{
				if (Curr.Y < Next.Y)
				{
					// inward corner, requires 3 tiles, the corner tile itself will have it's corner on top left.
					int Id = 1;
					int rotation = 0;
					FName TileName = FName("Corner_Convex");
					RoomCorners.Emplace(TileName, Id, rotation, Curr);
				}
				else
				{
					// outward corner, walls are on top and right side
					int Id = 1;
					int rotation = 0;
					FName TileName = FName("Corner_Concave");
					RoomCorners.Emplace(TileName, Id, rotation, Curr);
				}
			}
			else if (Curr.Y > Prev.Y)
			{
				if (Curr.X > Next.X)
				{
					// inward corner, requires 3 tiles, the corner tile will have it's corner on bottom left.
					int Id = 1;
					int rotation = 0;
					FName TileName = FName("Corner_Convex");
					RoomCorners.Emplace(TileName, Id, rotation, Curr);
				}
				else
				{
					// outward corner, walls are on the top and left.
					int Id = 1;
					int rotation = 0;
					FName TileName = FName("Corner_Concave");
					RoomCorners.Emplace(TileName, Id, rotation, Curr);
				}
			}
			else
			{
				if (Curr.X > Next.X)
				{
					// outward corner, walls are on the bottom and right
					int Id = 1;
					int rotation = 0;
					FName TileName = FName("Corner_Concave");
					RoomCorners.Emplace(TileName, Id, rotation, Curr);
				}
				else
				{
					// inward corner, requires 3 tiles, the corner tile will have it's corner on top right here.
					int Id = 1;
					int rotation = 0;
					FName TileName = FName("Corner_Convex");
					RoomCorners.Emplace(TileName, Id, rotation, Curr);
				}
			}
		}
		for (int i = 0; i < RoomCorners.Num(); i++)
		{
			FTileDescriptor NextTile = (RoomCorners.Num() - 1 == i) ? RoomCorners[0] : RoomCorners[i + 1];
			Curr = RoomCorners[i].Location;
			Next = NextTile.Location;
			uint32 CornerToCornerDist = FMath::Max(FMath::Abs(Curr.X - Next.X), FMath::Abs(Curr.Y - Next.Y));
			// subtract tilesize here because two adjacent concave corners won't need any adjoining walls.
			StraightWallCount = (CornerToCornerDist - TileSize) / TileSize;
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
						FName TileName = FName("Wall_Cave");
						int Id = 2;
						int Rotation = 270;
						FIntPoint Location = FIntPoint(Curr.X - TileSize, Curr.Y + (TileSize * j));
						WallDescriptors.Emplace(TileName, Id, Rotation, Location);
					}

				}
				else if (RoomCorners[i].Rotation == 90)
				{
					// walls here are up 1 tile and leading off to the right.
					for (int j = 0; j < StraightWallCount; j++)
					{
						FName TileName = FName("Wall_Cave");
						int Id = 2;
						int Rotation = 0;
						FIntPoint Location = FIntPoint(Curr.X + (TileSize * j), Curr.Y + TileSize);
						WallDescriptors.Emplace(TileName, Id, Rotation, Location);
					}
				}
				else if (RoomCorners[i].Rotation == 180)
				{
					// walls here are up 1 t ile and leading off to the left.
					for (int j = 0; j < StraightWallCount; j++)
					{
						FName TileName = FName("Wall_Cave");
						int Id = 2;
						int Rotation = 0;
						FIntPoint Location = FIntPoint(Curr.X - (TileSize * j), Curr.Y + TileSize);
						WallDescriptors.Emplace(TileName, Id, Rotation, Location);
					}
				}
				else if (RoomCorners[i].Rotation == 270)
				{
					// walls here are going up and are 1 to the right
					for (int j = 0; j < StraightWallCount; j++)
					{
						FName TileName = FName("Wall_Cave");
						int Id = 2;
						int Rotation = 90;
						FIntPoint Location = FIntPoint(Curr.X + TileSize, Curr.Y + (TileSize * j));
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
						FName TileName = FName("Wall_Cave");
						int Id = 2;
						int Rotation = 90;
						FIntPoint Location = FIntPoint(Curr.X, Curr.Y + (TileSize * (j + 1)));
						WallDescriptors.Emplace(TileName, Id, Rotation, Location);
					}
				}
				else if (RoomCorners[i].Rotation == 90)
				{
					for (int j = 0; j < StraightWallCount; j++)
					{
						FName TileName = FName("Wall_Cave");
						int Id = 2;
						int Rotation = 180;
						FIntPoint Location = FIntPoint(Curr.X + (TileSize * (j + 1)), Curr.Y);
						WallDescriptors.Emplace(TileName, Id, Rotation, Location);
					}
				}
				else if (RoomCorners[i].Rotation == 180)
				{
					for (int j = 0; j < StraightWallCount; j++)
					{
						FName TileName = FName("Wall_Cave");
						int Id = 2;
						int Rotation = 270;
						FIntPoint Location = FIntPoint(Curr.X, Curr.Y - (TileSize * (j + 1)));
						WallDescriptors.Emplace(TileName, Id, Rotation, Location);
					}
				}
				else if (RoomCorners[i].Rotation == 270)
				{
					for (int j = 0; j < StraightWallCount; j++)
					{
						FName TileName = FName("Wall_Cave");
						int Id = 2;
						int Rotation = 0;
						FIntPoint Location = FIntPoint(Curr.X - (TileSize * (j + 1)), Curr.Y);
						WallDescriptors.Emplace(TileName, Id, Rotation, Location);
					}
				}
			}
		}
		TileDescriptors.Append(RoomCorners);
		TileDescriptors.Append(WallDescriptors);
	}
}

void ASoulDrive2GameModeBase::PostLogin(APlayerController* NewPlayer)
{
	AGameModeBase::PostLogin(NewPlayer);
}

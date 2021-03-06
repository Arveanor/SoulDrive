// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "Runtime/Core/Public/Containers/Map.h"
#include "SDTileDescriptor.h"
#include "SoulDrive2GameModeBase.generated.h"

UENUM(BlueprintType)
enum class ProceduralTileEdges : uint8 {
	Open,
	Hall_Door,
	Hall_Open,
	Hall_Side,
	Wall_Main,
	Wall_Corner,
	Wall_Mid
};


USTRUCT(BlueprintType)
struct FActorLocation {
	GENERATED_USTRUCT_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ActorType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint RelativeLocation; // The blueprint may want to use offsets

	FActorLocation() {}
};

/*
** Describe a room to be placed, with an FIntPoint to define each corner, in clockwise order from the Minimum corner. Additionally may be used to contain info
** on the rooms visual and gameplay elements.
*/
USTRUCT(BlueprintType)
struct FRoomDescriptor {
	GENERATED_USTRUCT_BODY()

	public:
	UPROPERTY()
	TArray<FIntPoint> OrderedRoomCorners;

	UPROPERTY()
	int RoomId; // unique identifier of this room

	UPROPERTY()
	FBox Box; // just to be able to store our potential min/max for placement of floors.

	FRoomDescriptor() {}

	FORCEINLINE bool operator==(const FRoomDescriptor& other) const 
	{
		return RoomId == other.RoomId;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FRoomDescriptor& Other)
	{
		return GetTypeHash(Other.RoomId);
	}
};

USTRUCT(BlueprintType)
struct FRoomCluster {
	GENERATED_USTRUCT_BODY()

	public:
	UPROPERTY()
	TSet<FRoomDescriptor> Rooms;

	UPROPERTY()
	int ClusterId;

	UPROPERTY()
	FIntPoint Center;

	FRoomCluster() {}

	FORCEINLINE bool operator==(const FRoomCluster& other) const
	{
		return ClusterId == other.ClusterId;
	}
};

USTRUCT(BlueprintType)
struct FClusterPair {
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FRoomCluster Start;

	UPROPERTY()
	FRoomCluster End;

	UPROPERTY()
	uint32 Distance;

	FClusterPair() {}

	FORCEINLINE bool operator==(const FClusterPair& other) const
	{
		return Distance == other.Distance;
	}

	FORCEINLINE bool operator>(const FClusterPair& other) const
	{
		return Distance > other.Distance;
	}

	FORCEINLINE bool operator<(const FClusterPair& other) const
	{
		return Distance < other.Distance;
	}
};

USTRUCT(BlueprintType)
struct FTileEdgesTable : public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY()
	FName TileName;

	UPROPERTY()
	FName Edge1;

	UPROPERTY()
	FName Edge2;

	UPROPERTY()
	FName Edge3;

	UPROPERTY()
	FName Edge4;

	UPROPERTY()
	uint8 LocalId;

	FTileEdgesTable() {}

};

USTRUCT(BlueprintType)
struct FEdgeRelationshipTable : public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY()
	FName EdgeName;

	UPROPERTY()
	FName FriendName;

	FEdgeRelationshipTable() {}
};

USTRUCT(BlueprintType)
struct FIntPair {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Value;

	FIntPair(uint8 Key, uint8 Value)
		: Key(Key), Value(Value)
	{}

	FIntPair() {}
};

USTRUCT(BlueprintType)
struct FTileDescriptor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 LocalId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ProceduralTileEdges TopEdge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ProceduralTileEdges LeftEdge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ProceduralTileEdges RightEdge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ProceduralTileEdges BottomEdge;

	FTileDescriptor() { };

	FTileDescriptor(FName& inName, uint8 inLocalId, int inRotation, FIntPoint inLocation)
	{
		name = inName;
		LocalId = inLocalId;
		Rotation = inRotation;
		Location = inLocation;
	}

	// 	FTileDescriptor()
	// 		: name(TEXT("EmptySpace")), LocalId(0), rotation(0), TopEdge(ProceduralTileEdges::Open), RightEdge(ProceduralTileEdges::Open), BottomEdge(ProceduralTileEdges::Open), LeftEdge(ProceduralTileEdges::Open)
	// 	{ }
	// 
	// 	FTileDescriptor(FName& inName, uint8 LocalId, int inRotation, ProceduralTileEdges InTopEdge, ProceduralTileEdges InRightEdge, ProceduralTileEdges InBottomEdge, ProceduralTileEdges InLeftEdge)
	// 		: name(inName), rotation(inRotation), TopEdge(InTopEdge), RightEdge(InRightEdge), BottomEdge(InBottomEdge), LeftEdge(InLeftEdge)
	// 	{ }
};

class ASDNetPlayerControllerProxy;

/*
** We may need to track things here like FNames of Room Descriptors, or where exactly the openings are, or we may want to use a struct that only defines the
** array of points, that gets extended for the full room functionality, lot of this is TBD.
*/
USTRUCT(BlueprintType)
struct FHallwayDescriptor {
	GENERATED_USTRUCT_BODY()

	public:
	UPROPERTY()
	TArray<FIntPoint> OrderedHallCorners;

	UPROPERTY()
	TArray<FTileDescriptor> DoorLocations; // tiles where there are doors into this hallway.

	UPROPERTY()
	TArray<int> RoomIds; // the two rooms this hallway connects, could expand to include branching halls later

	FHallwayDescriptor() {}

	FORCEINLINE bool operator==(const FHallwayDescriptor& other) const
	{
		if (RoomIds.Num() < 2 || other.RoomIds.Num() < 2)
		{
			return false;
		}
		return RoomIds[0] == other.RoomIds[0] && RoomIds[1] == other.RoomIds[1];
	}
};

USTRUCT(BlueprintType)
struct FTileArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	TArray<ASDTileDescriptor*> TileArray;

	FTileArray() { }
};

USTRUCT(BlueprintType)
struct FEdgeAlignmentPair {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	ProceduralTileEdges Key;

	TSet<ProceduralTileEdges> Value;

	FEdgeAlignmentPair() { }
};

USTRUCT(BlueprintType)
struct FMapGenerationParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	int TileCountX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	int TileCountY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	TMap<int, int> ExactlyNIds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	TMap<ProceduralTileEdges, FTileArray> TilesByEdge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	TArray <int> ActorIds;

	// This needs to be created at map gen launch, based on available tiles and constants, I think
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	TArray<FEdgeAlignmentPair> EdgeMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	UDataTable* TileData;

	FMapGenerationParams() { }
};

/**
 *
 */
UCLASS()
class SOULDRIVE2_API ASoulDrive2GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;


public:
	UPROPERTY(BlueprintReadWrite, Category = "MapGen")
	int DEBUG_ROOM_LIMIT = 1112; // just for testing purposes, this will limit how many rooms get created, no matter how many leave quads exist.
	UPROPERTY(EditDefaultsOnly, Category="MapGen")
	uint32 MINIMUM_LEAF_QUAD_SIZE = 5;
 	UPROPERTY(EditDefaultsOnly, Category = "MapGen")
 	uint32 MINIMUM_STEM_QUAD_SIZE = 6;
 	UPROPERTY(EditDefaultsOnly, Category = "MapGen")
 	uint8 MAXIMUM_QUAD_DEPTH = 1;
	UPROPERTY(EditDefaultsOnly, Category = "MapGen")
 	uint32 MINIMUM_ROOM_DIMENSIONS = 3;

	TArray<AActor*> playerStartArray;

	ASoulDrive2GameModeBase();

	UPROPERTY(BlueprintReadWrite)
	int TileSize;

	template <typename EnumType>
	static FORCEINLINE EnumType GetEnumValueFromString(const FString& EnumName, const FString& String)
	{
		UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
		if (!Enum)
		{
			return EnumType(0);
		}
		return (EnumType)Enum->FindEnumIndex(FName(*String));
	}

	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value)
	{
		FString FullResult;
		FString *JustEnum = new FString();
		FString *EnumType = new FString();
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr)
		{
			return FString("Invalid");
		}
		FullResult = enumPtr->GetNameByValue((int64)Value).ToString();
		FullResult.Split("::", EnumType, JustEnum);
		return *JustEnum;
	}

	UFUNCTION(BlueprintCallable, Category = "Levels")
	void MovePlayerToSublevel(FVector Location, FName LevelName);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Levels")
	void GenerateLevel(const TArray<FName> &SubLevels, int MapTileCountX, int MapTileCountY);

	UFUNCTION(BlueprintCallable)
	bool GetIsLevelActive();

	UFUNCTION(BlueprintCallable)
	void SetLevelActive(bool IsLoaded);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Levels")
	void GenerateLevelActors(const TArray< TSubclassOf<AActor> > &LevelList, int MapTileCountX, int MapTileCountY);

	UFUNCTION(Category = "Levels")
	void AddControllerToTravelMap(ASDNetPlayerControllerProxy* Controller, FName MapName);

	/*
	** Driver function to get locational data for map layouts. Room and Hallway Descriptors will be used to return data that the caller can use to actually spawn
	** actors in that represent the data generated here. Expected to typically be called from each level blueprint on its own.
	** returns an int to indicate success or a specific failure code.
	*/
	UFUNCTION(BlueprintCallable, Category = "Levels")
	int GenerateMapData(UPARAM(ref) TArray<FTileDescriptor> &TileDescriptors, UPARAM(ref) TArray<FActorLocation> &Actors);

	// Called when a level finishes loading to spawn the actual player character and possess it over the dummy loading actor
	UFUNCTION(BlueprintCallable, Category = "Player")
	void SpawnPlayerCharacter(APlayerController *Player);

	int IndexFromPoint(int x, int y, int maxX, int maxY);
private:

	// How many cutouts do we want from our room? each index in this array should map to the number of cutouts to use, so if our weighted random technique gets a '1'
	// we want to use a single cutout. Note that '0' is expected to be valid, because of course we want some rooms that really are rectangles.
	const static TArray<uint16> NUMBER_OF_ROOM_CUTOUT_WEIGHTS;

	int32 GetListKeyByIndex(TDoubleLinkedList<int32> &List, int32 Index);

	/*
	** Returns the index of inEdge in the Map
	** inEdge: The ProceduralTileEdges member that is the key we are searching for in Map
	** Map: A map of ProceduralTileEdges, TArray<ProceduralTileEdges> stored as a TArray of a struct for blueprint compatability (thanks epic)
	*/
	uint8 GetEdgeMapIndex(ProceduralTileEdges inEdge, TArray<FEdgeAlignmentPair> Map);

	//Returns the Edge from Desc that faces direction, useful for figuring out where edges are for a rotated tile
	ProceduralTileEdges GetFacingEdge(int direction, FTileDescriptor* Desc);
	void CopyDescriptorData(FTileDescriptor* CopyFrom, FTileDescriptor* CopyTo);
	/*
	** Returns all valid rotations of TileToAdd in an array, should be called for every tile in a tile set on map generation
	** neighbors: array of ProceduralTileEdges that will face the location we are picking a new tile for. Ordering of the edges must be
	** Top, Right, Bottom, Left
	** TileToAdd: The tile we want to check for validity of
	** EdgeMap: A map of ProceduralTileEdges, TArray<ProceduralTileEdges> stored as a TArray of a struct for blueprint compatability (thanks epic)
	*/
	TArray<FTileDescriptor *> isValidForNeighbors(TArray<ProceduralTileEdges> neighbors, FTileDescriptor* TileToAdd, TArray<FEdgeAlignmentPair> & EdgeMap);
	void constructEdgeMap(TArray<FEdgeAlignmentPair> &EdgeMap);
	void constructTileSet(TArray<FTileDescriptor> &TileSet, UDataTable* TileData);

	/*
	** Make quads in a quadtree so that we can put rooms in them. All quads will be built with relative location values and then transformed by the Offset vector.
	** MapDimensions should be given as the size in number of tiles.
	*/
	void MakeQuads(FRandomStream RandomStream, FIntPoint MapDimensions, FVector Offset, uint8 RecursionCounter = 0);

	/*
	** Helper function to define a single quad when making all quads.
	*/
	FBox MakeQuad(uint8 Id, FIntPoint Slices, FIntPoint MapDimensions, FIntPoint MapOrigin);

	/*
	** Assign room dimensions within each quad.
	*/
	TArray<FRoomDescriptor> MakeRoomsInQuads(FRandomStream RandomStream);
	TArray<FHallwayDescriptor> MakeHallways(FRandomStream RandomStream, TArray<FRoomDescriptor> &Rooms);
	FTileDescriptor MakeDoorway(const TArray<FTileDescriptor>& CornerDescriptors, const FRoomDescriptor LocalRoom, const FRoomDescriptor ConnectedRoom);
	TArray<FTileDescriptor> PlaceWallTiles(const TArray<FTileDescriptor> &RoomCorners, const TArray<FTileDescriptor> &Doorways);
	void BuildTileLocationsList(TArray<FRoomDescriptor> Rooms, TArray<FHallwayDescriptor> Hallways, TArray<FTileDescriptor> &TileDescriptors);
	void PlaceFloorTiles(TArray<FTileDescriptor> &TileDescriptors, TArray<FTileDescriptor> WallDescriptors);

	void BuildActorList(const TArray<FRoomDescriptor> &Rooms, TArray<FActorLocation> &Actors);

	void PlaceHallTiles(TArray<FTileDescriptor> &TileDescriptors, FHallwayDescriptor Hallway);
	bool FindDoorConflict(const TArray<FTileDescriptor> &Doorways, const FIntPoint Location);
	void CalculateClusterDistances(TArray<FClusterPair>& DistanceArray, TArray <FRoomCluster>& Clusters);
	void CombineClusters(TArray<FClusterPair>& DistanceArray, TArray <FRoomCluster>& Clusters, TArray<FHallwayDescriptor> &Hallways);

	// After the first pass it is not necessarily true all rooms can be reached by all rooms. Here we fix that with
	// additional hallways to connect each cluster of rooms.
	void ConnectRoomClusters(TArray<FHallwayDescriptor> &Hallways, TArray<FRoomDescriptor> &Rooms);
	void CreateRoomCluster(TArray<FHallwayDescriptor>& Hallways, const FHallwayDescriptor &Hallway, FRoomCluster &Cluster, const TArray<FRoomDescriptor>& Rooms, int debugRecursion = 0);
	bool IsHallwayInAnyCluster(const FHallwayDescriptor &Hallway, const TArray<FRoomCluster> &Clusters);
	FIntPoint FindClusterCenter(FRoomCluster& Cluster);


	// Helper functions for making tile math less complicated
	FIntPoint FindTileOriginFromCenter(FIntPoint Center, int rotation);
	FIntPoint FindTileCenterFromOrigin(const FTileDescriptor Tile);

	// Use A* to calculate only centerpoints of hall tiles from Door to Door
	void CreateHallwayRoute(const FTileDescriptor& Start, const FTileDescriptor& End, TArray<FIntPoint>& Solution);

	// Helper for A* to tell us each neighbors score
	int ScoreHallwayStep(FIntPoint Cursor, FIntPoint Target, FIntPoint Candidate);

	// Use centerpoints of hall tiles to determine each tiles type and rotation
	TArray<FTileDescriptor> CreateHallTilesFromRoute(const TArray<FIntPoint>& Route);

	UDataTable* EdgeMapData;

	UPROPERTY()
	TArray<FBox> StemQuads;
	TArray<FBox> LeafQuads;

	UPROPERTY()
	bool IsLevelActive;

	TMap<FName, TArray<ASDNetPlayerControllerProxy*>> ControllersWaitingTravel;

protected:
	void PostLogin(APlayerController* NewPlayer) override;
};

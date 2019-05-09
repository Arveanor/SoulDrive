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
	int rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ProceduralTileEdges TopEdge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ProceduralTileEdges LeftEdge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ProceduralTileEdges RightEdge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ProceduralTileEdges BottomEdge;

	FTileDescriptor()
		: name(TEXT("EmptySpace")), LocalId(0), rotation(0), TopEdge(ProceduralTileEdges::Open), RightEdge(ProceduralTileEdges::Open), BottomEdge(ProceduralTileEdges::Open), LeftEdge(ProceduralTileEdges::Open)
	{ }

	FTileDescriptor(FName& inName, uint8 LocalId, int inRotation, ProceduralTileEdges InTopEdge, ProceduralTileEdges InRightEdge, ProceduralTileEdges InBottomEdge, ProceduralTileEdges InLeftEdge)
		: name(inName), rotation(inRotation), TopEdge(InTopEdge), RightEdge(InRightEdge), BottomEdge(InBottomEdge), LeftEdge(InLeftEdge)
	{ }
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
	TArray<AActor*> playerStartArray;

	ASoulDrive2GameModeBase();


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

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Levels")
	void GenerateLevel(const TArray<FName> &SubLevels, int MapTileCountX, int MapTileCountY);

	UFUNCTION(BlueprintCallable)
	bool GetIsLevelActive();

	UFUNCTION(BlueprintCallable)
	void SetLevelActive(bool IsLoaded);

	/****************************************************************************/
	/* Generates a list of ints representing the tiles to be loaded on a map.   
	** The list is 1 dimensional and must be mapped back to 2d space by the 
	** caller.
	** TileList is the output list containing all the tile references.
	** Params is a struct containing all of the necessary data about placeable
	** tiles and their relationships.                                           */
	/****************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Levels")
	int GenerateMapData(UPARAM(ref) TArray<FTileDescriptor> &TileList, UPARAM(ref) TArray<FIntPair> &ActorLocations, FMapGenerationParams Params);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Levels")
	void GenerateLevelActors(const TArray< TSubclassOf<AActor> > &LevelList, int MapTileCountX, int MapTileCountY);

	// Called when a level finishes loading to spawn the actual player character and possess it over the dummy loading actor
	UFUNCTION(BlueprintCallable, Category = "Player")
	void SpawnPlayerCharacter(APlayerController *Player);

	int IndexFromPoint(int x, int y, int maxX, int maxY);
private:
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

	UDataTable* EdgeMapData;
	
	UPROPERTY()
	bool IsLevelActive;

protected:
 	void PostLogin(APlayerController* NewPlayer) override;
};

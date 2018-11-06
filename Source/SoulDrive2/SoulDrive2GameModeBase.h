// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "Runtime/Core/Public/Containers/Map.h"
#include "SDTileDescriptor.h"
#include "SoulDrive2GameModeBase.generated.h"

USTRUCT(BlueprintType)
struct FTileArray
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	TArray<USDTileDescriptor*> TileArray;

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
	TArray<USDTileDescriptor *> TileSet;

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

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Levels")
	void GenerateLevel(const TArray<FName> &SubLevels, int MapTileCountX, int MapTileCountY);

	/****************************************************************************/
	/* Generates a list of ints representing the tiles to be loaded on a map.   
	** The list is 1 dimensional and must be mapped back to 2d space by the 
	** caller.
	** TileList is the output list containing all the tile references.
	** Params is a struct containing all of the necessary data about placeable
	** tiles and their relationships.                                           */
	/****************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Levels")
	void GenerateMapData(UPARAM(ref) TArray<USDTileDescriptor*> &TileList, FMapGenerationParams Params);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Levels")
	void GenerateLevelActors(const TArray< TSubclassOf<AActor> > &LevelList, int MapTileCountX, int MapTileCountY);

	// Called when a level finishes loading to spawn the actual player character and possess it over the dummy loading actor
	UFUNCTION(BlueprintCallable, Category = "Player")
	void SpawnPlayerCharacter(APlayerController *Player);

	int IndexFromPoint(int x, int y, int maxX, int maxY);
private:
	int32 GetListKeyByIndex(TDoubleLinkedList<int32> &List, int32 Index);
	uint8 GetEdgeMapIndex(ProceduralTileEdges inEdge, TArray<FEdgeAlignmentPair> Map);
};

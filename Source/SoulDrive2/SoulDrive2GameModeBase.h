// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "Runtime/Core/Public/Containers/Map.h"
#include "SoulDrive2GameModeBase.generated.h"

USTRUCT(BlueprintType)
struct FMapGenerationPair
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
		FName TileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
		int Frequency;

	FMapGenerationPair() { }
};

USTRUCT(BlueprintType)
struct FMapGenActorPair
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
		TSubclassOf <AActor> ActorTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
		int Frequency;

	FMapGenActorPair() { }
};

USTRUCT(BlueprintType)
struct FMapGenerationParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	TArray<FName> BasicTiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
		int TileCountX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
		int TileCountY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	TArray<FMapGenerationPair> ExactlyNTiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
		TArray<FMapGenActorPair> ExactlyNActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	TArray< TSubclassOf<AActor> > ActorTiles;

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

	UFUNCTION(BlueprintCallable, Category = "Levels")
	void GenerateMapData(UPARAM(ref) TArray< TSubclassOf<AActor> > &TileList, FMapGenerationParams Params);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Levels")
	void GenerateLevelActors(const TArray< TSubclassOf<AActor> > &LevelList, int MapTileCountX, int MapTileCountY);

	// Called when a level finishes loading to spawn the actual player character and possess it over the dummy loading actor
	UFUNCTION(BlueprintCallable, Category = "Player")
	void SpawnPlayerCharacter(APlayerController *Player);
};

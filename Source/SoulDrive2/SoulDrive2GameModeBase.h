// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "SoulDrive2GameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASoulDrive2GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	//virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

public:
	TArray<AActor*> playerStartArray;


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Levels")
	void GenerateLevel(const TArray<FName> &SubLevels, FName PersistentLevel, int TileSize);

};

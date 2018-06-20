// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/LevelScriptActor.h"
#include "SoulDrive2GameModeBase.h"
#include "SDIsmLevelBase.h"
#include "SDLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:

// 	ASDLevelScriptActor();
	ASDLevelScriptActor(const class FObjectInitializer& FOI);
	UFUNCTION(BlueprintCallable, Category = "Levels")
	void GenerateLevelFromStream();

	UFUNCTION()
	void GenerateMapData();
	
private:

 	UPROPERTY(BlueprintReadWrite, EditAnywhere, replicated, meta = (AllowPrivateAccess = "true"))
 	FRandomStream Stream;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Levels", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ASDIsmLevelBase> IsmParentBP;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FMapGenerationParams Params;
	TArray<int> TileList;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SDIsmLevelBase.generated.h"

UCLASS()
class SOULDRIVE2_API ASDIsmLevelBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDIsmLevelBase();

	UPROPERTY(BlueprintReadWrite, Category = "Levels")
	FTransform NextTransform;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Levels")
	int TileSize;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Levels")
	void LoadIsm(UPARAM(ref) TArray<int> &TileTypes, int SizeX, int SizeY);

private:
	
};

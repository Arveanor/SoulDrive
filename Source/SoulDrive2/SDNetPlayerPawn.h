// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SDBasePawn.h"
#include "SDNetPlayerPawn.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDNetPlayerPawn : public ASDBasePawn
{
	GENERATED_BODY()
	
public:
	ASDNetPlayerPawn();

	UFUNCTION(BlueprintCallable, Category = "Levels")
	void TravelToLevel(FName LevelToLoad);
	
	void EquipItem(FString MeshName);
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};

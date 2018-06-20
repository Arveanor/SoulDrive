// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SDCommonUtilities.generated.h"

UCLASS()
class SOULDRIVE2_API ASDCommonUtilities : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDCommonUtilities();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Levels", meta = (WorldContext = "WorldContextObject"))
	static void AllControllersPostLevelLoad(UObject *WorldContextObject);
	
	
};

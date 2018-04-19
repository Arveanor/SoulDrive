// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "SDTravelTrigger.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDTravelTrigger : public ATriggerBox
{
	GENERATED_BODY()
	
protected:
	void BeginPlay() override;
	
public:
	ASDTravelTrigger();
	
	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
	void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Level")
	FName LevelToLoad;
};

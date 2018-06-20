// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "Blueprint/UserWidget.h"
#include "SDGameState.generated.h"


/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDGameState : public AGameState
{
	GENERATED_BODY()


public:

	ASDGameState();

	UPROPERTY(BlueprintReadWrite, Category = "Loading")
	TSubclassOf<class UUserWidget> LoadingScreen;

	UFUNCTION(BlueprintCallable, Category = "Loading")
	void SetIsMapGenerationCompleted(bool generationFlag);

	UFUNCTION(BlueprintCallable, Category = "Loading")
	bool GetIsMapGenerationCompleted();
	
private:

	bool IsMapGenerationCompleted;

};

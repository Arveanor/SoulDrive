// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SDBaseAIController.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDBaseAIController : public AAIController
{
	GENERATED_BODY()
	
	ASDBaseAIController(const FObjectInitializer &FOI);

	UPROPERTY(transient)
	UBlackboardComponent *BlackboardComp;

	UPROPERTY(transient)
	UBehaviorTreeComponent *BehaviorComp;

public:

 	//virtual void Possess(class APawn *InPawn) override;
 	void setEnemy(class APawn *InPawn);
 
	UFUNCTION(BlueprintCallable, Category = Behavior)
	void searchForEnemy();

protected:
	uint8 EnemyKeyID;
	uint8 EnemyLocationID;
	
};

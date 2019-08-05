// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SDDotSpell.generated.h"

UCLASS()
class SOULDRIVE2_API ASDDotSpell : public ASDBaseSpell
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDDotSpell(const class FObjectInitializer& FOI);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};

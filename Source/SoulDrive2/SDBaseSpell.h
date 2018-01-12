// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SDBaseSpell.generated.h"

UCLASS()
class SOULDRIVE2_API ASDBaseSpell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDBaseSpell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	APawn* Caster;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void CastSpell(FVector target);
};

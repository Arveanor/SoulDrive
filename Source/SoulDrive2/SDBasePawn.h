// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "SDBasePawn.generated.h"

UCLASS()
class SOULDRIVE2_API ASDBasePawn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASDBasePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	TArray<FName> SpellsList;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Spells")
	virtual void CastSpell(FName SpellName);
};

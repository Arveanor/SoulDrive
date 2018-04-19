// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SDBaseSpell.h"
#include "SDProjectile.h"
#include "SDCheatSpell.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDCheatSpell : public ASDBaseSpell
{
	GENERATED_BODY()
	
public:
	void Init(APawn* OwnedBy);

protected:
	
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player, meta = (AllowPrivateAccess = "true"))
	FString CheatProjectileMesh;

	virtual void CastSpell(FVector target) override;

	float ProjectileVelocity;
};

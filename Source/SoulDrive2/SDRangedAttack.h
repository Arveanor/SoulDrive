// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SDBaseSpell.h"
#include "SDRangedAttack.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDRangedAttack : public ASDBaseSpell
{
	GENERATED_BODY()
	
public:
	void HandleTarget(AActor *Target, bool IsAlly) override;
	UPROPERTY(EditAnywhere, Category = "Projectile")
	UMaterial *ProjectileMat;

protected:
	virtual void BeginPlay() override;
	
private:
	virtual void CastSpell(FVector target) override;
	FString AttackMeshName;
	float ProjectileSpeed;


};

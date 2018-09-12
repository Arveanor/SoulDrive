// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SDBaseSpell.h"
#include "SDCelestialFragmentSpell.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDCelestialFragmentSpell : public ASDBaseSpell
{
	GENERATED_BODY()
	
public:
	ASDCelestialFragmentSpell(const class FObjectInitializer& FOI);
	
	void Init(AController* OwnedBy) override;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	UMaterial *ProjectileMat;

	UAnimSequence *Anim;

	void HandleTarget(AActor *Target, bool IsAlly) override;
protected:
	virtual void BeginPlay() override;

private:
	virtual void CastSpell(FVector target) override;

	FString ProjectileMeshName;
	float ProjectileSpeed;
};

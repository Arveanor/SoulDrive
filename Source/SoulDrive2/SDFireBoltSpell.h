// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SDBaseSpell.h"
#include "SDFireBoltSpell.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDFireBoltSpell : public ASDBaseSpell
{
	GENERATED_BODY()

public:
	ASDFireBoltSpell(const class FObjectInitializer& FOI);

	UPROPERTY(EditAnywhere, Category = "Projectile")
	UMaterial *ProjectileMat;

	UAnimSequence *Anim;

	void HandleTarget(AActor *Target, bool IsAlly) override;
protected:
	virtual void BeginPlay() override;

private:
	virtual void CastSpell(FVector target) override;
	
	FString FireboltMeshName;
	float ProjectileSpeed;

};

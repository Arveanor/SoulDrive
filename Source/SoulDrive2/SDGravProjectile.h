// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SDProjectile.h"
#include "SDGravProjectile.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDGravProjectile : public ASDProjectile
{
	GENERATED_BODY()
	
public:
	// Sets default values for this pawn's properties
	ASDGravProjectile(const class FObjectInitializer& FOI);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Category = "Spells")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
private:
	float GravRange;
	FVector VelocityDueToGravity;
	FVector ProjToPlayerUnit;
	const float GRAV_ACCELERATION = 160.f;
};

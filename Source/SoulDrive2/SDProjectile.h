// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "SDBaseSpell.h"
#include "SDProjectile.generated.h"

UCLASS()
class SOULDRIVE2_API ASDProjectile : public APawn
{
	GENERATED_BODY()

public:

	ASDProjectile(const class FObjectInitializer& FOI);
	void Init(FString MeshName, float Velocity, UMaterialInstance* Mat, FVector InForward);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(replicated)
	ASDBaseSpell* ParentSpell;

	UPROPERTY(replicated)
	int testInteger;

	float ProjectileSpeed;
	FVector ProjectileVelocity;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Category = "Spells")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void SetParentSpell(ASDBaseSpell *Parent);
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Player, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComp;

	UPROPERTY()
	UMaterialInstanceDynamic* Material_Dyn;
	
	FString StaticMeshName;

	UPROPERTY()
	FVector TargetForward;
};

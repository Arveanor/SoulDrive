// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SDBaseSpell.h"
#include "SDBaseAoE.generated.h"

UCLASS()
class SOULDRIVE2_API ASDBaseAoE : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDBaseAoE();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Category = "Spells")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, replicated, Category = "Spells")
	UParticleSystemComponent *ParticleComp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, replicated, Category = "Spells")
	UCapsuleComponent *CapsuleComp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, replicated, Category = "Spells")
	UDecalComponent *DecalComp;

	void SetParentSpell(ASDBaseSpell *InParentSpell);

private:
	FTimerHandle TimerHandler;
	
	UFUNCTION()
	void OnPrimed();

	UFUNCTION()
	void OnFinished();

	UPROPERTY()
	ASDBaseSpell *ParentSpell;

	UPROPERTY()
	UMaterialInterface* MatInterface;
};

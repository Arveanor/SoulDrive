// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SDBasePawn.h"
#include "SDConstants.h"
#include "SDBaseSpell.generated.h"

UCLASS()
class SOULDRIVE2_API ASDBaseSpell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDBaseSpell(const class FObjectInitializer& FOI);

	virtual void Init(AController* OwnedBy);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void CooldownTimerCallback();

	//Used to queue up cast animations and interrupt movement
	virtual bool PreCast();
	UPROPERTY(replicated)
	bool OnCooldown;
	UPROPERTY(replicated)
	float CooldownLength;
	UPROPERTY(replicated)
	AController* Caster;
	UPROPERTY(replicated)
	ACharacter* CasterCharacter;

	int ManaCost;
	int TeamId;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void CastSpell(FVector target);
	virtual void HandleTarget(AActor *Target, bool IsAlly);
	AController* GetCaster();
	ACharacter* GetCasterCharacter();

	void SetTeamId(int InTeamId);
	int GetTeamId();
	bool isOnCooldown();
	WeaponRequirements WeaponType;

	UPROPERTY(replicated, BlueprintReadWrite, EditAnywhere, Category = "cooldown")
	FTimerHandle TimerHandler;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "BehaviorTree/BehaviorTree.h"
#include "SDBaseAIController.h"
#include "SDBaseSpell.h"
#include "SDBasePawn.h"
#include "SDBaseAICharacter.generated.h"

UCLASS()
class SOULDRIVE2_API ASDBaseAICharacter : public ASDBasePawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASDBaseAICharacter();

	virtual void CastSpell(FName SpellName, FVector AimedAt) override;

	UFUNCTION(BlueprintCallable, Category = "Spells")
	void CooldownFinished();

	UFUNCTION(BlueprintCallable, Category = "Spells")
	bool GetIsSpellOnCooldown();

	UPROPERTY(EditAnywhere, Category = Behavior)
	class UBehaviorTree *BotBehavior;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	ASDBaseSpell* Sunburst;
	float SunburstCooldown;
	bool IsSunburstOnCooldown;

	FTimerHandle TimerHandler;
		
};

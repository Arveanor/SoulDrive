// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "SDTeamIdentity.h"
#include "SDBasePawn.generated.h"

UCLASS()
class SOULDRIVE2_API ASDBasePawn : public ACharacter, public ISDTeamIdentity
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASDBasePawn();
	void SetPlayerID(int id);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	TArray<FName> SpellsList;

	int PlayerId;

	int MaxHp;
	float CurrentHp;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Spells")
	virtual void CastSpell(FName SpellName, FVector AimedAt);

	UFUNCTION(BlueprintCallable, Category = "Player")
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Team")
	int TeamId;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Team")
	int GetTeamId();
	virtual int GetTeamId_Implementation() override;

	void SetTimeId(int InTeamId);

	float GetHpRatio();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SDBaseAICharacter.h"
#include "SDBaseAiSpawner.generated.h"

UCLASS()
class SOULDRIVE2_API ASDBaseAiSpawner : public AActor//, public ISDTeamIdentity
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDBaseAiSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	FTimerHandle TimerHandler;

	UFUNCTION(Category = "Timer")
	void ReceiveSpawnTick();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

 	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enemies")
	TSubclassOf<class ASDBaseAICharacter> EnemyType;
 	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enemies")
	TSubclassOf<class ASDBaseAIController> ControllerType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Timer")
	float TickRate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Placement")
	float SpawnAtDistance;
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComp;
};

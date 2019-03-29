// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDSunBurstSpell.h"
#include "SDBasePawn.h"


// Sets default values
ASDBasePawn::ASDBasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ASDBasePawn::SetPlayerID(int id)
{
	PlayerId = id;
}

int ASDBasePawn::GetPlayerID()
{
	return PlayerId;
}

// Called when the game starts or when spawned
void ASDBasePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASDBasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASDBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASDBasePawn::CastSpell(FName SpellName, FVector AimedAt)
{
// 	FActorSpawnParameters SpawnInfo;
// 	ASDSunBurstSpell *Sunburst = GetWorld()->SpawnActor<ASDSunBurstSpell>(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnInfo);
// 	Sunburst->CastSpell(AimedAt);

}

float ASDBasePawn::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	CurrentHp -= Damage;

	return 0.0f;
}

int ASDBasePawn::GetTeamId_Implementation()
{
	return TeamId;
}

void ASDBasePawn::SetTimeId(int InTeamId)
{
	TeamId = InTeamId;
}

float ASDBasePawn::GetHpRatio()
{
	return CurrentHp / MaxHp;
}

float ASDBasePawn::GetManaRatio()
{
	return CurrentMana / MaxMana;
}

float ASDBasePawn::GetCurrentMana()
{
	return CurrentMana;
}

void ASDBasePawn::BurnMana(int amount)
{
	CurrentMana -= amount;
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDCheatSpell.h"



void ASDCheatSpell::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("Cheat Spell constructor"));
	if (CheatProjectileMesh.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("CheatProjectileMesh being defaulted"));
		CheatProjectileMesh = "StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'";
	}
	ProjectileVelocity = 450.0f;
}


void ASDCheatSpell::Init(AController* OwnedBy)
{
	Caster = OwnedBy;
}

void ASDCheatSpell::CastSpell(FVector target)
{
	UE_LOG(LogTemp, Warning, TEXT("SpellSlot0 being cast in cheat spell class"));
	if (Caster != nullptr)
	{
// 		FActorSpawnParameters SpawnInfo;
// 		TSubclassOf<AActor> TargetClass = ASDProjectile::StaticClass();
// 		FVector SpawnAt = Caster->GetTransform().GetLocation();
// 		FRotator SpawnRotation = Caster->GetBaseAimRotation();
// 		SpawnAt.X += Caster->GetTransform().GetRotation().GetAxisX().X * 55.0f;
// 		SpawnAt.Y += Caster->GetTransform().GetRotation().GetAxisY().Y * 55.0f;
// 		FTransform SpawnTransform(SpawnRotation, SpawnAt);
// 		auto ResultActor = Cast<ASDProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, TargetClass, SpawnTransform));
// 		if (ResultActor != nullptr)
// 		{
// 			ResultActor->Init(CheatProjectileMesh, ProjectileVelocity);
// 			UGameplayStatics::FinishSpawningActor(ResultActor, SpawnTransform);
// 		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not cast spell because caster is null"));
	}
}
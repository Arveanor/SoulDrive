// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDCelestialFragmentSpell.h"
#include "SDGravProjectile.h"




ASDCelestialFragmentSpell::ASDCelestialFragmentSpell(const class FObjectInitializer& FOI)
	: Super (FOI)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	ProjectileMeshName = "temporary_name";
	ProjectileSpeed = 200.f;
}

void ASDCelestialFragmentSpell::HandleTarget(AActor *Target, bool IsAlly)
{
	const FDamageEvent DamageEvent;
	if (GetInstigator() != nullptr)
	{
		Target->TakeDamage(10.0f, DamageEvent, GetInstigator()->GetController(), this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("instigator was null"));
	}
}

void ASDCelestialFragmentSpell::BeginPlay()
{
	Super::BeginPlay();
	OnCooldown = false;
	WeaponType = WeaponRequirements::AnyWeapon;
}

void ASDCelestialFragmentSpell::CastSpell(FVector target)
{
	if (Caster != nullptr)
	{
// 		ASDNetPlayerPawn* CasterCharacter = dynamic_cast<ASDNetPlayerPawn *> (Caster->GetPawn());
// 		if (CasterCharacter != nullptr)
// 		{
// 			CasterCharacter->SetIsCasting(true);
// 		}
		Caster->StopMovement();
		FActorSpawnParameters SpawnInfo;
		TSubclassOf<AActor> TargetClass = ASDGravProjectile::StaticClass();
		FVector SpawnAt = Caster->GetPawn()->GetTransform().GetLocation();
		FVector ToTarget = target - SpawnAt;
		FRotator SpawnRotation = ToTarget.Rotation();
		SpawnAt.X += Caster->GetPawn()->GetTransform().GetRotation().GetAxisX().X * 55.0f;
		SpawnAt.Y += Caster->GetPawn()->GetTransform().GetRotation().GetAxisY().Y * 55.0f;
		FTransform SpawnTransform(SpawnRotation, SpawnAt);
		auto ResultActor = Cast<ASDGravProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, TargetClass, SpawnTransform));
		if (ResultActor != nullptr)
		{
			//ResultActor->Init(ProjectileMeshName, ProjectileSpeed, nullptr);
			UGameplayStatics::FinishSpawningActor(ResultActor, SpawnTransform);
			ResultActor->SetParentSpell(this);
		}
	}
	else
	{

	}
}

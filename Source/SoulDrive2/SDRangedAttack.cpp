// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDRangedAttack.h"




void ASDRangedAttack::HandleTarget(AActor *Target, bool IsAlly)
{

}

void ASDRangedAttack::BeginPlay()
{
	OnCooldown = false;
	WeaponType = WeaponRequirements::RangedOnly;
}

void ASDRangedAttack::CastSpell(FVector target)
{
	if (Caster != nullptr)
	{
		if (PreCast())
		{
			FActorSpawnParameters SpawnInfo;
			TSubclassOf<AActor> TargetClass = ASDProjectile::StaticClass();
			FVector SpawnAt = Caster->GetPawn()->GetTransform().GetLocation();
			FRotator SpawnRotation = Caster->GetPawn()->GetBaseAimRotation();
			SpawnAt.X += Caster->GetPawn()->GetTransform().GetRotation().GetAxisX().X * 55.0f;
			SpawnAt.Y += Caster->GetPawn()->GetTransform().GetRotation().GetAxisY().Y * 55.0f;
			FTransform SpawnTransform(SpawnRotation, SpawnAt);
			auto ResultActor = Cast<ASDProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, TargetClass, SpawnTransform));
			if (ResultActor != nullptr)
			{
				ResultActor->Init(AttackMeshName, ProjectileSpeed, ProjectileMat);
				UGameplayStatics::FinishSpawningActor(ResultActor, SpawnTransform);
				ResultActor->SetParentSpell(this);
			}
		}
	}
	else
	{

	}
}

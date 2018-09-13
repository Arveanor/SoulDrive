// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDGravProjectile.h"

ASDGravProjectile::ASDGravProjectile(const class FObjectInitializer& FOI)
	: Super(FOI)
{
	GravRange = 900.f;
	ProjectileSpeed = 200.f;
}

void ASDGravProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ASDGravProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ParentSpell->GetCasterCharacter() == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Caster Character is null"));
		return;
	}

	if ((GetActorLocation() - ParentSpell->GetCasterCharacter()->GetActorLocation()).Size() < GravRange)
	{
		/* Calculate the new vector due to "gravity" and add it to the existing vector. Change in velocity due to acceleration should be linear, easy to calculate
		** Need to get the unit vector from the projectile to the player (since the player is the center of gravity in this case)
		*/
		ProjToPlayerUnit = ParentSpell->GetCasterCharacter()->GetActorLocation() - GetActorLocation();
		ProjToPlayerUnit.Normalize();
		VelocityDueToGravity = DeltaTime * GRAV_ACCELERATION * ProjToPlayerUnit;
		VelocityDueToGravity.Z = 0.f; // we don't want to change the z!
		float magnitude = ProjectileVelocity.Size();
		ProjectileVelocity += VelocityDueToGravity;
		SetActorRotation(ProjectileVelocity.Rotation(), ETeleportType::None);
		//UE_LOG(LogTemp, Warning, TEXT("ParentSpell caster location = %s"), *ParentSpell->GetInstigator()->GetActorLocation().ToString());
	}
}

void ASDGravProjectile::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin entered for SDProjectile"));
	if (OtherActor->Implements<ISDTeamIdentity::UClassType>() && ParentSpell != nullptr)
	{
		ParentSpell->HandleTarget(OtherActor, (ISDTeamIdentity::Execute_GetTeamId(OtherActor) == ParentSpell->GetTeamId()));
	}
	else
	{
		if (ParentSpell == nullptr) UE_LOG(LogTemp, Warning, TEXT("ParentSpell is null"));
	}
}

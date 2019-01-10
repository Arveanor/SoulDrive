// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDSlash.h"



void ASDSlash::CastSpell(FVector target)
{
	if (Caster != nullptr)
	{
		if (!PreCast())
		{
			return;
		}
		TArray<TWeakObjectPtr<AActor>> AlreadyHitPawns;
		FVector PlayerLocation = Caster->GetPawn()->GetActorLocation();
		FVector LookAtVector;
		FRotator LookAtRotator(0.f, GetInitialRotation(), 0.f);
		const FName TraceTag("MyTraceTag");

		GetWorld()->DebugDrawTraceTag = TraceTag;

		TArray<FHitResult> TraceHits;

		FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
		TraceParams.bTraceComplex = true;
		TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = false;
		TraceParams.TraceTag = TraceTag;

		target.Z = PlayerLocation.Z;
		LookAtVector = target - PlayerLocation;
		LookAtVector.Normalize();
		LookAtVector *= TRACE_LENGTH;
		FRotationMatrix LookAtRotationMatrix(LookAtRotator);
		LookAtVector = LookAtRotationMatrix.TransformVector(LookAtVector);

		FCollisionObjectQueryParams ObjectList;
		ObjectList.AddObjectTypesToQuery(ECC_Pawn);

		for (int i = 0; i < NUM_TRACES; i++)
		{
			LookAtRotator.Yaw = TRACE_ANGLE;
			FRotationMatrix LookAtRotationMatrix(LookAtRotator);
			LookAtVector = LookAtRotationMatrix.TransformVector(LookAtVector);
			target = LookAtVector + PlayerLocation;

			bool success = GetWorld()->LineTraceMultiByObjectType(TraceHits, PlayerLocation, target, ObjectList, TraceParams);
			if (success)
			{
				const FDamageEvent DamageEvent;
				for (FHitResult hit : TraceHits)
				{
					if (hit.Actor->IsValidLowLevel() && !AlreadyHitPawns.Contains(hit.Actor) && hit.Actor->Implements<ISDTeamIdentity::UClassType>())
					{
						if (ISDTeamIdentity::Execute_GetTeamId(hit.Actor.Get()) != this->GetTeamId())
						{
							hit.Actor->TakeDamage(10.f, DamageEvent, Caster, Caster->GetPawn());
							AlreadyHitPawns.Add(hit.Actor);
						}
					}
				}
			}
		}
	}
}

void ASDSlash::BeginPlay()
{
	OnCooldown = false;
	WeaponType = WeaponRequirements::MeleeOnly;
}

float ASDSlash::GetInitialRotation()
{
	if (NUM_TRACES % 2 == 0)
	{
		return (TRACE_ANGLE * NUM_TRACES / 2) * -1 - TRACE_ANGLE;
	}
	else
	{
		return (TRACE_ANGLE * NUM_TRACES / 2) * -1 - TRACE_ANGLE / 2;
	}
}

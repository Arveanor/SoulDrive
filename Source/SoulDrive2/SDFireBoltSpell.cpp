// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDFireBoltSpell.h"
#include "SDProjectile.h"


ASDFireBoltSpell::ASDFireBoltSpell(const class FObjectInitializer& FOI)
	: Super (FOI)
{
	FireboltMeshName = TEXT("Mesh Name");
	ProjectileSpeed = 450.f;

	static ConstructorHelpers::FObjectFinder<UAnimSequence> anim(TEXT("AnimSequence'/Game/SDContent/Characters/Blob_Man/Blob_Man_Skeletal_Anim_Armature_Idle_Animation_Idle_Animation.Blob_Man_Skeletal_Anim_Armature_Idle_Animation_Idle_Animation'"));
	Anim = anim.Object;
}

void ASDFireBoltSpell::Init(AController* OwnedBy)
{
	Caster = OwnedBy;
}

void ASDFireBoltSpell::HandleTarget(AActor *Target, bool IsAlly)
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

void ASDFireBoltSpell::BeginPlay()
{
	
}

void ASDFireBoltSpell::CastSpell(FVector target)
{
	if (Caster != nullptr)
	{
		ASDNetPlayerPawn* SDCasterCharacter = dynamic_cast<ASDNetPlayerPawn *> (CasterCharacter);
		if (SDCasterCharacter != nullptr)
		{
			SDCasterCharacter->SetIsCasting(true);
		}
		Caster->StopMovement();
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
			ResultActor->Init(FireboltMeshName, ProjectileSpeed, ProjectileMat);
			UGameplayStatics::FinishSpawningActor(ResultActor, SpawnTransform);
			ResultActor->SetParentSpell(this);
		}
	}
	else
	{

	}
}

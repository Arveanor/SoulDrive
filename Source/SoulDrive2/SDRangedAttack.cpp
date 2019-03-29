// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDRangedAttack.h"




ASDRangedAttack::ASDRangedAttack(const class FObjectInitializer& FOI)
	: Super(FOI)
{
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MatInst(TEXT("MaterialInstanceConstant'/Game/SDContent/VFX/Gunshot/MAT_Bullet_Inst.MAT_Bullet_Inst'"));
	Material = (UMaterialInstance*)MatInst.Object;

	ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particles"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSys1(TEXT("ParticleSystem'/Game/SDContent/VFX/Gunshot/PFX_RIBBON_ATTACH.PFX_RIBBON_ATTACH'"));
	ParticleSys = dynamic_cast<UParticleSystem *>(ParticleSys1.Object);
	ParticleComp->SetTemplate(ParticleSys1.Object);

	ProjectileSpeed = 550.f;
	AttackMeshName = TEXT("AttackMesh");
}

void ASDRangedAttack::HandleTarget(AActor *Target, bool IsAlly)
{
	const FDamageEvent DamageEvent;
	if (GetInstigator() != nullptr && !IsAlly)
	{
		Target->TakeDamage(10.0f, DamageEvent, GetInstigator()->GetController(), this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("instigator was null"));
	}
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
			FVector Forward = target - SpawnAt;
			Forward.Normalize();
			FRotator SpawnRotation = Caster->GetPawn()->GetBaseAimRotation();
			SpawnAt.X += Caster->GetPawn()->GetTransform().GetRotation().GetAxisX().X * 55.0f;
			SpawnAt.Y += Caster->GetPawn()->GetTransform().GetRotation().GetAxisY().Y * 55.0f;
			FTransform SpawnTransform(SpawnRotation, SpawnAt);
			auto ResultActor = Cast<ASDProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, TargetClass, SpawnTransform));
			if (ResultActor != nullptr)
			{
				ResultActor->Init(AttackMeshName, ProjectileSpeed, Material, Forward);
				UGameplayStatics::FinishSpawningActor(ResultActor, SpawnTransform);
				ResultActor->SetParentSpell(this);

				UGameplayStatics::SpawnEmitterAttached(ParticleSys, ResultActor->GetRootComponent());
			}
		}
	}
	else
	{

	}
}

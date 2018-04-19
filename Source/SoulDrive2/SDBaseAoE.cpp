// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDBaseSpell.h"
#include "SDBasePawn.h"
#include "SDTeamIdentity.h"
#include "SDBaseAoE.h"


// Sets default values
ASDBaseAoE::ASDBaseAoE()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particlesz"));
	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	RootComponent = CapsuleComp;
	ParticleComp->SetupAttachment(CapsuleComp);
	DecalComp->SetupAttachment(CapsuleComp);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MI(TEXT("Material'/Game/SDContent/Spells/Decals/MAT_Decal_GroundTargetRing.MAT_Decal_GroundTargetRing'"));
		
	ParticleComp->SetTemplate(PS.Object);
	DecalComp->SetDecalMaterial(MI.Object);
	FQuat MyQuat(0.0f, 90.0f, 0.0f, 1.0f);
	DecalComp->SetRelativeRotation(MyQuat);
	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &ASDBaseAoE::OnOverlapBegin);
	//CapsuleComp->SetRelativeScale3D(FVector(5.0f, 5.0f, 10.0f));
}

// Called when the game starts or when spawned
void ASDBaseAoE::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(TimerHandler, this, &ASDBaseAoE::OnPrimed, .5f, false, 1.5f);
 	SetActorTickEnabled(false);
 	SetActorEnableCollision(false);
	ParticleComp->DeactivateSystem();

	DecalComp->SetLifeSpan(1.5f);

}

// Called every frame
void ASDBaseAoE::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASDBaseAoE::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// Here we need to determine if the pawn(s) in this aoe are allies/enemies, and then execute the aoe's attached effect
	UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin entered for SDBaseAoE"));
	if (OtherActor->Implements<ISDTeamIdentity::UClassType>() && ParentSpell != nullptr)
	{
		ParentSpell->HandleTarget(OtherActor, (ISDTeamIdentity::Execute_GetTeamId(OtherActor) == ParentSpell->GetTeamId()));
	}
	else
	{
		if (ParentSpell == nullptr) UE_LOG(LogTemp, Warning, TEXT("ParentSpell is null"));
	}
}

void ASDBaseAoE::SetParentSpell(ASDBaseSpell *InParentSpell)
{
	ParentSpell = InParentSpell;
}

void ASDBaseAoE::OnPrimed()
{
	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
	ParticleComp->ActivateSystem(false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandler, this, &ASDBaseAoE::OnFinished, 0.25f, false, 0.25f);
}

void ASDBaseAoE::OnFinished()
{
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
}

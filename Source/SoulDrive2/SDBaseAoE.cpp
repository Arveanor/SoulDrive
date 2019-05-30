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
	bReplicates = true;
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particlesz"));
	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	RootComponent = CapsuleComp;
	ParticleComp->SetupAttachment(CapsuleComp);
	DecalComp->SetupAttachment(CapsuleComp);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MI(TEXT("Material'/Game/SDContent/Spells/Decals/MAT_Decal_GroundTargetRing.MAT_Decal_GroundTargetRing'"));
	
	ParticleComp->SetTemplate(PS.Object);
	MatInterface = MI.Object;
	DecalComp->SetMaterial(0, MI.Object);
	FQuat MyQuat(0.0f, 90.0f, 0.0f, 1.0f);
	DecalComp->SetRelativeRotation(MyQuat);
	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &ASDBaseAoE::OnOverlapBegin);

	//CapsuleComp->SetRelativeScale3D(FVector(5.0f, 5.0f, 10.0f));
}

// Called when the game starts or when spawned
void ASDBaseAoE::BeginPlay()
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Heyo Jay-o"));
	}

	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(TimerHandler, this, &ASDBaseAoE::OnPrimed, .35f, false, -1.0f);
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

void ASDBaseAoE::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASDBaseAoE, CapsuleComp);
	DOREPLIFETIME(ASDBaseAoE, DecalComp);
	DOREPLIFETIME(ASDBaseAoE, ParticleComp);

}

void ASDBaseAoE::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// Here we need to determine if the pawn(s) in this aoe are allies/enemies, and then execute the aoe's attached effect
	UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin entered for SDBaseAoE"));
	if (OtherActor->Implements<ISDTeamIdentity::UClassType>() && ParentSpell != nullptr)
	{
		ISDTeamIdentity *IdActor = dynamic_cast<ISDTeamIdentity *>(OtherActor);
		int otherId = IdActor->Execute_GetTeamId(OtherActor);
		bool allied = otherId == ParentSpell->GetTeamId();
		ParentSpell->HandleTarget(OtherActor, allied);
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
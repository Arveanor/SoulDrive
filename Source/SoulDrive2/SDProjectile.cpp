// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "Net/UnrealNetwork.h"
#include "SDProjectile.h"


// Sets default values
// ASDProjectile::ASDProjectile()
// {
//  	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
// 	PrimaryActorTick.bCanEverTick = true;
// 
// 	bReplicates = true;
// 
// 	ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particles"));
// 	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
// 
// 	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSys(TEXT("ParticleSystem'/Game/SDContent/VFX/Fireball/PFX_Fireball_Sparks_01.PFX_Fireball_Sparks_01'"));
// 	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjMesh(TEXT("'/Engine/BasicShapes/Plane.Plane'"));
// 	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MatInst(TEXT("MaterialInstanceConstant'/Game/SDContent/VFX/Fireball/MAT_Fireball_01_Inst.MAT_Fireball_01_Inst'"));
// 
// 	Material = (UMaterialInstance*)MatInst.Object;
// 	ParticleComp->SetTemplate(ParticleSys.Object);
// 	MeshComp->SetStaticMesh(ProjMesh.Object);
// 	ParticleComp->SetupAttachment(MeshComp);
// 	RootComponent = MeshComp;
// 
// //	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
// 	MeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
// 
// 	Material_Dyn = UMaterialInstanceDynamic::Create(Material, MeshComp);
// 	MeshComp->SetMaterial(0, Material_Dyn);
// 
// 	MeshComp->OnComponentBeginOverlap.AddDynamic(this, &ASDProjectile::OnOverlapBegin);
// }


ASDProjectile::ASDProjectile(const class FObjectInitializer& FOI)
	: Super(FOI)
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particles"));
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSys(TEXT("ParticleSystem'/Game/SDContent/VFX/Fireball/PFX_Fireball_Sparks_01.PFX_Fireball_Sparks_01'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjMesh(TEXT("'/Engine/BasicShapes/Plane.Plane'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MatInst(TEXT("MaterialInstanceConstant'/Game/SDContent/VFX/Fireball/MAT_Fireball_01_Inst.MAT_Fireball_01_Inst'"));

	Material = (UMaterialInstance*)MatInst.Object;
	ParticleComp->SetTemplate(ParticleSys.Object);
	MeshComp->SetStaticMesh(ProjMesh.Object);
	ParticleComp->SetupAttachment(MeshComp);
	RootComponent = MeshComp;

	//	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	Material_Dyn = UMaterialInstanceDynamic::Create(Material, MeshComp);
	MeshComp->SetMaterial(0, Material_Dyn);

	MeshComp->OnComponentBeginOverlap.AddDynamic(this, &ASDProjectile::OnOverlapBegin);
}

void ASDProjectile::Init(FString MeshName, float Velocity, UMaterialInterface *Mat)
{
	StaticMeshName = MeshName;
	ProjectileSpeed = Velocity;
}

void ASDProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ASDProjectile, ParentSpell);
	DOREPLIFETIME(ASDProjectile, testInteger);
}

// Called when the game starts or when spawned
void ASDProjectile::BeginPlay()
{
	Super::BeginPlay();
	ProjectileVelocity = ProjectileSpeed * GetActorForwardVector();
	ProjectileVelocity.Z = 0.f;
}

// Called every frame
void ASDProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector MoveBy = ProjectileVelocity * DeltaTime;
	SetActorLocation(GetActorLocation() + MoveBy);
}

void ASDProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

// Called to bind functionality to input
void ASDProjectile::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASDProjectile::SetParentSpell(ASDBaseSpell *Parent)
{
	ParentSpell = Parent;
	testInteger = 130;
}
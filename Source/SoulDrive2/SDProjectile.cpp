// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "Net/UnrealNetwork.h"
#include "SDProjectile.h"


ASDProjectile::ASDProjectile(const class FObjectInitializer& FOI)
	: Super(FOI)
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(true);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjMesh(TEXT("'/Engine/BasicShapes/Plane.Plane'"));

	MeshComp->SetStaticMesh(ProjMesh.Object);
	//MeshComp->SetIsReplicated(true);

	RootComponent = MeshComp;

	MeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	MeshComp->OnComponentBeginOverlap.AddDynamic(this, &ASDProjectile::OnOverlapBegin);
}

void ASDProjectile::Init(FString MeshName, float Velocity, UMaterialInstance* Mat, FVector InForward)
{
	StaticMeshName = MeshName;
	TargetForward = InForward;
	ProjectileSpeed = Velocity;
	Material_Dyn = UMaterialInstanceDynamic::Create(Mat, MeshComp);
	MeshComp->SetMaterial(0, Material_Dyn);
	NetMatChange(Mat);
}

void ASDProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASDProjectile, ParentSpell);
	DOREPLIFETIME(ASDProjectile, MeshComp);
}

// Called when the game starts or when spawned
void ASDProjectile::BeginPlay()
{
	Super::BeginPlay();
	ProjectileVelocity = ProjectileSpeed * TargetForward;
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
}

void ASDProjectile::NetMatChange_Implementation(UMaterialInstance* Mat)
{
	Material_Dyn = UMaterialInstanceDynamic::Create(Mat, MeshComp);
	MeshComp->SetMaterial(0, Material_Dyn);
}
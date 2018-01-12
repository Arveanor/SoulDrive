// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDProjectile.h"


// Sets default values
ASDProjectile::ASDProjectile()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	const TCHAR* tStaticMeshName = *StaticMeshName;
//	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjMesh(tStaticMeshName);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	MeshComp->SetStaticMesh(ProjMesh.Object);
	RootComponent = MeshComp;
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASDProjectile::Init(FString MeshName, float Velocity)
{
	StaticMeshName = MeshName;
	ProjectileSpeed = Velocity;
}

// Called when the game starts or when spawned
void ASDProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASDProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector ForwardVector = GetActorForwardVector();
	FVector ProcessingPosition = GetActorLocation();
	ProcessingPosition += ForwardVector * ProjectileSpeed * DeltaTime;
	SetActorLocation(ProcessingPosition);
}

// Called to bind functionality to input
void ASDProjectile::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


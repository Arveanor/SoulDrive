// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDBaseEquipment.h"


// Sets default values
ASDBaseEquipment::ASDBaseEquipment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMesh(TEXT("StaticMesh'/Game/SDContent/Equippables/Armor/Spiked_Shoulder/MDL_Spiked_Shoulder_01.MDL_Spiked_Shoulder_01'"));
	MeshComp->SetStaticMesh(ItemMesh.Object);
	RootComponent = MeshComp;

	MeshComp->OnComponentBeginOverlap.AddDynamic(this, &ASDBaseEquipment::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ASDBaseEquipment::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void ASDBaseEquipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASDBaseEquipment::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap event fired!"));
}

void ASDBaseEquipment::SetActiveInWorld(bool active)
{
	this->SetActorTickEnabled(active);
	this->SetActorEnableCollision(active);
	this->SetActorHiddenInGame(!active);
}

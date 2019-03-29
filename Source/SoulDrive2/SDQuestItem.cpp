// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDQuestItem.h"


// Sets default values
ASDQuestItem::ASDQuestItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMesh(TEXT("StaticMesh'/Game/SDContent/Equippables/Armor/Spiked_Shoulder/MDL_Spiked_Shoulder_01.MDL_Spiked_Shoulder_01'"));
	MeshComp->SetStaticMesh(ItemMesh.Object);
	RootComponent = MeshComp;

//	MeshComp->OnComponentBeginOverlap.AddDynamic(this, &ASDQuestItem::OnOverlapBegin);

}

// Called when the game starts or when spawned
void ASDQuestItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASDQuestItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


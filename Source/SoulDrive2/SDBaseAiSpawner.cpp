// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDBaseAiSpawner.h"


// Sets default values
ASDBaseAiSpawner::ASDBaseAiSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));

	PrimaryActorTick.bCanEverTick = true;
	TickRate = 10.0f;
	SpawnAtDistance = 75.0f;
	EnemyType = ASDBaseAICharacter::StaticClass();
	ControllerType = ASDBaseAIController::StaticClass();
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMesh(TEXT("StaticMesh'/Game/SDContent/Equippables/Armor/Spiked_Shoulder/MDL_Spiked_Shoulder_01.MDL_Spiked_Shoulder_01'"));
	MeshComp->SetStaticMesh(ItemMesh.Object);
	RootComponent = MeshComp;

}

// Called when the game starts or when spawned
void ASDBaseAiSpawner::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(TimerHandler, this, &ASDBaseAiSpawner::ReceiveSpawnTick, TickRate, true, TickRate);
}

float ASDBaseAiSpawner::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	this->Destroy(false, true);
	return 0.0f;
}

void ASDBaseAiSpawner::ReceiveSpawnTick()
{
	ASDBaseAICharacter *SpawnedCharacter;
	ASDBaseAIController *SpawnedController;

	FVector SpawnAt = this->GetActorForwardVector() * SpawnAtDistance;
	SpawnAt += this->GetActorLocation();

	FActorSpawnParameters SpawnInfo;
	SpawnedCharacter = GetWorld()->SpawnActor<ASDBaseAICharacter>(EnemyType, SpawnAt, this->GetActorRotation(), SpawnInfo);
	SpawnedController = GetWorld()->SpawnActor<ASDBaseAIController>(ControllerType, SpawnAt, this->GetActorRotation(), SpawnInfo);

	if (SpawnedCharacter != nullptr && SpawnedController != nullptr)
	{
		SpawnedController->Possess(SpawnedCharacter);
	}
}

// Called every frame
void ASDBaseAiSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


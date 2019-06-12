// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDPortal.h"


// Sets default values
ASDPortal::ASDPortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));

	Box->OnComponentBeginOverlap.AddDynamic(this, &ASDPortal::OnBoxBeginOverlap);
	Box->SetBoxExtent(FVector(100.f, 100.f, 100.f), true);
}

bool ASDPortal::ReceiveClick_Implementation(ASDNetPlayerControllerProxy* Controller)
{
	Controller->SetInteractionTarget(this);
	return true;
}

void ASDPortal::OnBoxBeginOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ASDNetPlayerPawn* OverlapPawn = dynamic_cast<ASDNetPlayerPawn *>(OtherActor);
	if (OverlapPawn != nullptr)
	{
		if (OverlapPawn->GetInteractionTarget() == this)
		{
			OverlapPawn->TravelToLevel(TargetLevel);
		}
	}
}

// Called when the game starts or when spawned
void ASDPortal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASDPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASDPortal::SetExistingURL_Implementation(const FString &InURL)
{
	ExistingURL = InURL;
}

bool ASDPortal::SetExistingURL_Validate(const FString &InURL)
{
	return true;
}


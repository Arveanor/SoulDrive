// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDGameInstance.h"
#include "SDNetPlayerPawn.h"


void ASDNetPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	USDGameInstance *GameInstance = dynamic_cast<USDGameInstance *>(GetGameInstance());
	if (GameInstance != nullptr)
	{
		if (GameInstance->GetIsOnlineSession())
		{
			CarriedItems = GameInstance->GetPlayerInventory(PlayerId);
		}
		else
		{
			CarriedItems = GameInstance->GetPlayerInventory(0);
		}
	}
}

ASDNetPlayerPawn::ASDNetPlayerPawn()
{
	this->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ASDNetPlayerPawn::OnOverlapBegin);
	this->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASDNetPlayerPawn::OnHitDetected);
}

void ASDNetPlayerPawn::TravelToLevel(FName LevelToLoad)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempting to do ServerTravel"));
	if (HasAuthority())
	{
		bool result = GetWorld()->ServerTravel(LevelToLoad.ToString());
		if (result)
		{
			UE_LOG(LogTemp, Warning, TEXT("returned true"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("returned false"));
		}
	}
}

void ASDNetPlayerPawn::EquipItem(FString MeshName)
{

}

void ASDNetPlayerPawn::SetProxyController(APlayerController *ClientController)
{
	ProxyController = ClientController;
}

void ASDNetPlayerPawn::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Server pawn overlap event!"));
	ASDBaseEquipment *GroundItem =  dynamic_cast<ASDBaseEquipment *>(OtherActor);
	CarriedItems->Add(GroundItem);
	GroundItem->SetActiveInWorld(false);
	
	USDGameInstance *GameInstance = dynamic_cast<USDGameInstance *>(GetGameInstance());
	if (GameInstance != nullptr)
	{
		GameInstance->OnItemPickup.Broadcast(GroundItem);
	}
}

void ASDNetPlayerPawn::OnHitDetected(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("pawn collision fired! %s"), *OtherActor->GetName());
}
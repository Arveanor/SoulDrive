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
	MaxHp = 100;
	CurrentHp = (float)MaxHp;
	TeamId = 1;
}

void ASDNetPlayerPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASDNetPlayerPawn, IsMoving);
}

ASDNetPlayerPawn::ASDNetPlayerPawn()
{
	this->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ASDNetPlayerPawn::OnOverlapBegin);
	this->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASDNetPlayerPawn::OnHitDetected);

	MainWeapons.SetNumZeroed(2, true);
	AltWeapons.SetNumZeroed(2, true);
	CurrentWeaponSet = &MainWeapons;

	IsSpellCasting = false;
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

bool ASDNetPlayerPawn::DropItem(ASDBaseEquipment *Equipment)
{
	if (CarriedItems->Find(Equipment))
	{
		CarriedItems->Remove(Equipment);
		return true;
	}
	else
	{
		return false;
	}
}

void ASDNetPlayerPawn::EquipItem(ASDBaseEquipment *Item, uint8 Slot)
{
	FName SocketName;
	if (Item != nullptr)
	{
		switch (Slot)
		{
		case(EEquipSlot::MainWeaponMainHand):
			if (CurrentWeaponSet != &MainWeapons)
			{
				SwapWeapons();
			}
			SetMainWeapon(Item, true);
			SocketName = TEXT("Hand_R_endSocket");
			break;
		case(EEquipSlot::AltWeaponMainHand):
			if (CurrentWeaponSet == &MainWeapons)
			{
				SwapWeapons();
			}
			SetAltWeapon(Item, true);
			SocketName = TEXT("Hand_R_endSocket");
			break;
		case(EEquipSlot::Shoulder):
			SocketName = TEXT("Shoulder_LSocket");
		}
		Item->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}
}

void ASDNetPlayerPawn::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Server pawn overlap event!"));
	ASDBaseEquipment *GroundItem = dynamic_cast<ASDBaseEquipment *>(OtherActor);
	if (GroundItem != nullptr)
	{
		CarriedItems->Add(GroundItem);
		GroundItem->SetActiveInWorld(false);
		USDGameInstance *GameInstance = dynamic_cast<USDGameInstance *>(GetGameInstance());
		if (GameInstance != nullptr)
		{
			GameInstance->OnItemPickup.Broadcast(GroundItem);
		}
	}
}

void ASDNetPlayerPawn::OnHitDetected(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("pawn collision fired! %s"), *OtherActor->GetName());
}

bool ASDNetPlayerPawn::IsCasting()
{
	return IsSpellCasting;
}

void ASDNetPlayerPawn::SwapWeapons()
{
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, false);
	if (CurrentWeaponSet == &MainWeapons)
	{
		CurrentWeaponSet = &AltWeapons;
		if (MainWeapons[0] != nullptr)
		{
			MainWeapons[0]->SetActorHiddenInGame(true);
		}
		if (AltWeapons[0] != nullptr)
		{
			AltWeapons[0]->SetActorHiddenInGame(false);
		}
		//EquipItem(AltWeapons[0], (uint8)EEquipSlot::AltWeaponMainHand);
	}
	else
	{
		CurrentWeaponSet = &MainWeapons;
		if (AltWeapons[0] != nullptr)
		{
			AltWeapons[0]->SetActorHiddenInGame(true);
		}
		if (MainWeapons[0] != nullptr)
		{
			MainWeapons[0]->SetActorHiddenInGame(false);
		}
		//EquipItem(MainWeapons[0], (uint8)EEquipSlot::MainWeaponMainHand);
	}
}

void ASDNetPlayerPawn::SetAltWeapon(ASDBaseEquipment * Weapon, bool bMainHand)
{
	AltWeapons[bMainHand ? 0 : 1] = Weapon;
}

void ASDNetPlayerPawn::SetIsCasting(bool isCasting)
{
	IsSpellCasting = isCasting;
}

void ASDNetPlayerPawn::SetMainWeapon(ASDBaseEquipment *Weapon, bool bMainHand)
{
	MainWeapons[bMainHand ? 0 : 1] = Weapon;
}
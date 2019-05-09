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
			CarriedItems = ConvertItemStruct( GameInstance->GetPlayerInventory(PlayerId).InventoryArray);
		}
		else
		{
			CarriedItems = ConvertItemStruct(GameInstance->GetPlayerInventory(0).InventoryArray);
			for (ASDBaseEquipment* Item : CarriedItems)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("ASDNetPlayerPawn::BeginPlay: Adding item to player inventory")));
				if (Item->EquippedStatus > 0)
				{
					GameInstance->OnItemEquipped.Broadcast(Item, (Item->EquippedStatus == 1) ? true : false);
				}
				else
				{
					GameInstance->OnItemPickup.Broadcast(Item);
				}
			}
		}
	}
	MaxHp = 100;
	MaxMana = 100;
	CurrentHp = (float)MaxHp;
	CurrentMana = (float)MaxMana;
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
			UGameplayStatics::OpenLevel(GetWorld(), LevelToLoad);
			UE_LOG(LogTemp, Warning, TEXT("returned false"));
		}
	}
}

bool ASDNetPlayerPawn::DropItem(ASDBaseEquipment *Equipment)
{
	if (CarriedItems.Find(Equipment))
	{
		CarriedItems.Remove(Equipment);
		return true;
	}
	else
	{
		return false;
	}
}

void ASDNetPlayerPawn::EquipItem(ASDBaseEquipment *Item, uint8 Slot)
{
	USDGameInstance* GameInstance = dynamic_cast<USDGameInstance *>(GetGameInstance());
	uint8 EquippedStatus = 1;
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
			SetMainWeapon(dynamic_cast<ASDBaseWeapon*> (Item), true);
			SocketName = TEXT("Hand_R_endSocket");
			break;
		case(EEquipSlot::AltWeaponMainHand):
			EquippedStatus = 2;
			if (CurrentWeaponSet == &MainWeapons)
			{
				SwapWeapons();
			}
			SetAltWeapon(dynamic_cast<ASDBaseWeapon*> (Item), true);
			SocketName = TEXT("Hand_R_endSocket");
			break;
		case(EEquipSlot::Shoulder):
			SocketName = TEXT("Shoulder_LSocket");
		}
		if (GameInstance != nullptr)
		{
			GameInstance->SetItemEquipped(PlayerId, Item, EquippedStatus);
			Item->EquippedStatus = EquippedStatus;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to mark item as equipped due to null game instance"));
		}
		Item->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}
}

void ASDNetPlayerPawn::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ASDBaseEquipment *GroundItem = dynamic_cast<ASDBaseEquipment *>(OtherActor);
	if (GroundItem != nullptr)
	{
		CarriedItems.Add(GroundItem);
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

void ASDNetPlayerPawn::SetInteractionTarget(AActor* Target)
{
	InteractionTarget = Target;
}

AActor* ASDNetPlayerPawn::GetInteractionTarget()
{
	return InteractionTarget;
}

void ASDNetPlayerPawn::SwapWeapons()
{
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, false);
	if (CurrentWeaponSet == &MainWeapons)
	{
		CurrentWeaponSet = &AltWeapons;
		if (MainWeapons.Num() > 0)
		{
			MainWeapons[0]->SetActorHiddenInGame(true);
		}
		if (AltWeapons.Num() > 0)
		{
			AltWeapons[0]->SetActorHiddenInGame(false);
		}
		//EquipItem(AltWeapons[0], (uint8)EEquipSlot::AltWeaponMainHand);
	}
	else
	{
		CurrentWeaponSet = &MainWeapons;
		if (AltWeapons.Num() > 0)
		{
			AltWeapons[0]->SetActorHiddenInGame(true);
		}
		if (MainWeapons.Num() > 0)
		{
			MainWeapons[0]->SetActorHiddenInGame(false);
		}
		//EquipItem(MainWeapons[0], (uint8)EEquipSlot::MainWeaponMainHand);
	}
}

ASDBaseWeapon* ASDNetPlayerPawn::GetMainWeapon()
{
	if (MainWeapons.Num() > 0)
	{
		return MainWeapons[0];
	}
	else
	{
		return nullptr;
	}
}

ASDBaseWeapon* ASDNetPlayerPawn::GetAltWeapon()
{
	if (AltWeapons.Num() > 0)
	{
		return AltWeapons[0];
	}
	else
	{
		return nullptr;
	}
}

TArray<ASDBaseEquipment *> ASDNetPlayerPawn::ConvertItemStruct(const TArray<FItemStruct> &StructList)
{
	TArray<ASDBaseEquipment *> Result;
	ASDBaseEquipment* Equipment;
	ASDBaseWeapon *Weapon;
	UStaticMesh *ItemMesh;

	for (FItemStruct Item : StructList)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Instigator = this;
		if (Item.WeaponType != WeaponRequirements::NotAWeapon)
		{
			Weapon = GetWorld()->SpawnActor<ASDBaseWeapon>(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnInfo);
			Weapon->WeaponType = Item.WeaponType;
			Equipment = Weapon;
		}
		else
		{
			Equipment = GetWorld()->SpawnActor<ASDBaseEquipment>(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnInfo);
		}
		ItemMesh = LoadObject<UStaticMesh>(nullptr, *Item.MeshName);
		Equipment->SetStaticMesh(ItemMesh);
		Equipment->SetActiveInWorld(false);
		if (Item.isEquipped == 1)
		{
			Equipment->SetActorHiddenInGame(false);
			switch (Item.ItemType)
			{
			case(EItemType::Shoulder):
				EquipItem(Equipment, (uint8)EEquipSlot::Shoulder);
				break;
			case(EItemType::Weapon):
				EquipItem(Equipment, (uint8)EEquipSlot::MainWeaponMainHand);
				break;
			}
		}
		Equipment->SetActorScale3D(FVector(7.f, 7.f, 7.f));
		Equipment->ItemName = Item.ItemName;
		Equipment->IType = Item.ItemType;
		Equipment->EquippedStatus = Item.isEquipped;
		Equipment->MeshName = Item.MeshName;
		Result.Add(Equipment);
	}
	return Result;
}

void ASDNetPlayerPawn::SetAltWeapon(ASDBaseWeapon * Weapon, bool bMainHand)
{
	AltWeapons.Insert(Weapon, bMainHand ? 0 : 1);
}

void ASDNetPlayerPawn::SetIsCasting(bool isCasting)
{
	IsSpellCasting = isCasting;
}

bool ASDNetPlayerPawn::UnequipItem(ASDBaseEquipment *TargetItem)
{
	USDGameInstance* GameInstance = dynamic_cast<USDGameInstance *>(GetGameInstance());

	bool success = false;
	switch (TargetItem->IType)
	{
	case (EItemType::Weapon):
		ASDBaseWeapon *Weapon = dynamic_cast<ASDBaseWeapon *>(TargetItem);
		if (nullptr != Weapon)
		{
			if (MainWeapons.Contains(Weapon))
			{
				MainWeapons.Remove(Weapon);
				success = true;
			}
			if (AltWeapons.Contains(Weapon))
			{
				AltWeapons.Remove(Weapon);
				success = true;
			}
		}
		break;
	}
	if (success)
	{
		if (GameInstance != nullptr)
		{
			GameInstance->SetItemEquipped(PlayerId, TargetItem, 0);
			TargetItem->EquippedStatus = 0;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to mark item as equipped due to null game instance"));
		}
	}
	return success;
}

void ASDNetPlayerPawn::SetMainWeapon(ASDBaseWeapon *Weapon, bool bMainHand)
{
	MainWeapons.Insert(Weapon, bMainHand ? 0 : 1);
}
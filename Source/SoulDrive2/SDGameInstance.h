// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "SDBaseEquipment.h"
#include "SDBasePawn.h"
#include "SDGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryPickupDelegate, ASDBaseEquipment*, PickedUp );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryDropDelegate, ASDBaseEquipment*, Dropped );

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API USDGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	
public:
	USDGameInstance();
	void ActorLoaded();

	void GetPlayerIDOnJoin(ASDBasePawn *Player);
	
	UPROPERTY(BlueprintAssignable, Category = "Test")
	FTestDelegate OnTestDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FInventoryPickupDelegate OnItemPickup;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FInventoryDropDelegate OnItemDropped;

	TArray<ASDBaseEquipment *> *GetPlayerInventory(int PlayerId);
	bool GetIsOnlineSession();
	void OnlineSessionCreated();

private:
	FCriticalSection ServerCriticalSection;

	int CriticalFunctionsCount;
	int PlayersConnected;
	bool IsOnlineSession;
	TArray<TArray<ASDBaseEquipment *> *> PlayerInventoryArray;
	int NextPlayerID;
};

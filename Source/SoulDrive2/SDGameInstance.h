// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "SDBaseWeapon.h"
#include "SDBasePawn.h"
#include "SDBaseQuest.h"
#include "SDGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryPickupDelegate, ASDBaseEquipment*, PickedUp );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryDropDelegate, ASDBaseEquipment*, Dropped );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryEquipDelegate, ASDBaseEquipment*, Equipped, bool, MainHand );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerCharLoaded, uint8, PlayerId );


USTRUCT(BlueprintType)
struct FPlayerInventory {
	GENERATED_BODY()

	UPROPERTY()
	TArray<FItemStruct> InventoryArray;
	
	UPROPERTY()
	uint8 PlayerId;

	FPlayerInventory() {}
};

USTRUCT(BlueprintType)
struct FPlayerQuests {
	GENERATED_BODY()

	UPROPERTY()
	TArray<FQuestStruct> QuestArray;

	UPROPERTY()
	uint8 PlayerId;

	FPlayerQuests() {}
};

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

	uint8 GetPlayerIDOnJoin();
	
	UPROPERTY(BlueprintAssignable, Category = "Test")
	FTestDelegate OnTestDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FInventoryPickupDelegate OnItemPickup;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FInventoryDropDelegate OnItemDropped;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FInventoryEquipDelegate OnItemEquipped;

	UPROPERTY(BlueprintAssignable, Category = "Player")
	FServerCharLoaded OnServerCharLoaded;

	FPlayerQuests GetPlayerQuests(int PlayerId);
	int AddQuestListForPlayer(int PlayerId);
	void AddQuestToPlayer(int PlayerId, USDBaseQuest* Quest);

	FPlayerInventory GetPlayerInventory(int PlayerId);
	void AddItemToPlayerInventory(int PlayerId, ASDBaseEquipment* Item);
	int AddInventoryForPlayer(int PlayerId);
	void SetItemEquipped(int PlayerId, ASDBaseEquipment* Item, uint8 Status);
	void RemoveItemFromPlayerInventory(int PlayerId, ASDBaseEquipment* Item);
	bool GetIsOnlineSession();
	void OnlineSessionCreated();

	FCriticalSection ServerCriticalSection;

private:

	int CriticalFunctionsCount;
	int PlayersConnected;
	bool IsOnlineSession;

	UPROPERTY()
	TArray<FPlayerInventory> PlayerInventoryArray;

	UPROPERTY()
	TArray<FPlayerQuests> PlayerQuestArray;

	UPROPERTY()
	uint8 NextPlayerID = 0;

	int FindPlayerInventory(int PlayerId);
	int FindPlayerQuests(int PlayerId);
};

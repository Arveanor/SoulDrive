// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDGameInstance.h"




USDGameInstance::USDGameInstance()
{
	IsOnlineSession = false;
}

void USDGameInstance::ActorLoaded()
{

}

uint8 USDGameInstance::GetPlayerIDOnJoin()
{
	NextPlayerID++;
	return NextPlayerID;
}

FPlayerQuests USDGameInstance::GetPlayerQuests(int PlayerId)
{
	int idx = FindPlayerQuests(PlayerId);
	if (idx < 0)
	{
		idx = AddQuestListForPlayer(PlayerId);
	}
	if (idx >= 0)
	{
		return PlayerQuestArray[idx];
	}
	else
	{
		FPlayerQuests Quests;
		Quests.PlayerId = -1;
		return Quests;
	}
}

int USDGameInstance::AddQuestListForPlayer(int PlayerId)
{
	FPlayerQuests Quests;
	Quests.PlayerId = PlayerId;
	return PlayerQuestArray.Add(Quests);
}

void USDGameInstance::AddQuestToPlayer(int PlayerId, USDBaseQuest* Quest)
{
	int idx = FindPlayerQuests(PlayerId);
	FQuestStruct QuestStruct;
	QuestStruct.QuestName = Quest->QuestName;
	QuestStruct.Status = Quest->QuestStatus;
	QuestStruct.RequiredItemName = Quest->GetRequiredItemName();

	if (idx < 0)
	{
		idx = AddQuestListForPlayer(PlayerId);
	}
	if (idx >= 0)
	{
		PlayerQuestArray[idx].QuestArray.Add(QuestStruct);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to add Quest to Game Instance Quest List for Player %d"), PlayerId);
	}
}

FPlayerInventory USDGameInstance::GetPlayerInventory(int PlayerId)
{
	int idx = FindPlayerInventory(PlayerId);
	if (idx < 0)
	{
		idx = AddInventoryForPlayer(PlayerId);
	}
	if (idx >= 0)
	{
		return PlayerInventoryArray[idx];
	}
	else
	{
		FPlayerInventory Inventory;
		Inventory.PlayerId = -1;
		return Inventory;
	}
}

void USDGameInstance::AddItemToPlayerInventory(int PlayerId, ASDBaseEquipment* Item)
{

	int idx = FindPlayerInventory(PlayerId);
	FItemStruct ItemStruct;
	ItemStruct.ItemName = Item->ItemName;
	ItemStruct.ItemType = Item->IType;
	ItemStruct.MeshName = Item->MeshName;
	ASDBaseWeapon* Weapon = dynamic_cast<ASDBaseWeapon *>(Item);
	if (Weapon != nullptr)
	{
		ItemStruct.WeaponType = Weapon->WeaponType;
	}
	else
	{
		ItemStruct.WeaponType = WeaponRequirements::NotAWeapon;
	}

	if (idx < 0)
	{
		idx = AddInventoryForPlayer(PlayerId);
	}
	if (idx >= 0)
	{
		PlayerInventoryArray[idx].InventoryArray.AddUnique(ItemStruct);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to add Item to Game Instance Inventory for Player %d"), PlayerId);
	}
}

int USDGameInstance::AddInventoryForPlayer(int PlayerId)
{
	FPlayerInventory Inventory;
	Inventory.PlayerId = PlayerId;
	return PlayerInventoryArray.Add(Inventory);
}

void USDGameInstance::SetItemEquipped(int PlayerId, ASDBaseEquipment* Item, uint8 Status)
{
	FString DebugMessage;
	for (int i = 0; i < PlayerInventoryArray[FindPlayerInventory(PlayerId)].InventoryArray.Num(); i++)
	{
// 		DebugMessage = FString(TEXT("Element "));
// 		DebugMessage += FString::FromInt(i);
// 		DebugMessage += FString(TEXT(" [Name: "));
// 		DebugMessage += LocalItem.ItemName.ToString();
// 		DebugMessage += FString(TEXT(", Equipped: "));
// 		DebugMessage += FString::FromInt(LocalItem.isEquipped);
// 		DebugMessage += FString(TEXT(")"));

//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, DebugMessage);

		if (PlayerInventoryArray[FindPlayerInventory(PlayerId)].InventoryArray[i].ItemName == Item->ItemName)
		{
			PlayerInventoryArray[FindPlayerInventory(PlayerId)].InventoryArray[i].isEquipped = Status;
		}
	}
}

void USDGameInstance::RemoveItemFromPlayerInventory(int PlayerId, ASDBaseEquipment* Item)
{
	TArray<FItemStruct> Inventory = PlayerInventoryArray[FindPlayerInventory(PlayerId)].InventoryArray;
	int i = 0;
	for (FItemStruct LocalItem : Inventory)
	{
		if (LocalItem.ItemName == Item->ItemName)
		{
			break;
		}
		i++;
	}
	Inventory.RemoveAt(i);
}

bool USDGameInstance::GetIsOnlineSession()
{
	return IsOnlineSession;
}

void USDGameInstance::OnlineSessionCreated()
{
	IsOnlineSession = true;
}

int USDGameInstance::FindPlayerInventory(int PlayerId)
{
	int i = 0;
	for (FPlayerInventory fpi : PlayerInventoryArray)
	{
		if (fpi.PlayerId == PlayerId) return i;
		i++;
	}
	return -1;
}

int USDGameInstance::FindPlayerQuests(int PlayerId)
{
	int i = 0;
	for (FPlayerQuests Quests : PlayerQuestArray)
	{
		if (Quests.PlayerId == PlayerId) return i;
		i++;
	}
	return -1;
}

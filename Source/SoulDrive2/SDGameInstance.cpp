// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDGameInstance.h"




USDGameInstance::USDGameInstance()
{
	PlayerInventoryArray.Add(new TArray<ASDBaseEquipment *>());

	IsOnlineSession = false;
}

void USDGameInstance::ActorLoaded()
{

}

TArray<ASDBaseEquipment*> *USDGameInstance::GetPlayerInventory(int PlayerId)
{
	return PlayerInventoryArray[PlayerId];
}

bool USDGameInstance::GetIsOnlineSession()
{
	return IsOnlineSession;
}

void USDGameInstance::OnlineSessionCreated()
{
	IsOnlineSession = true;
}

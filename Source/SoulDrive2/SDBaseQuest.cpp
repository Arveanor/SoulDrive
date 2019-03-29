// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDBaseQuest.h"




// Sets default values
USDBaseQuest::USDBaseQuest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	QuestName = FName("Secret Quest");
	RequiredItemName = FName("Secret Container");
}

bool USDBaseQuest::TurnInItem(ASDBaseEquipment *Item)
{
	if (Item->ItemName == RequiredItemName)
	{
		//GetWorld()->DestroyActor(Item);
		return true;
	}
	else
	{
		return false;
	}
}

FName USDBaseQuest::GetRequiredItemName()
{
	return RequiredItemName;
}

void USDBaseQuest::SetRequiredItemName(FName ItemName)
{
	RequiredItemName = ItemName;
}


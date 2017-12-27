// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "AI/Navigation/NavigationSystem.h"
#include "SoulDrive2GameModeBase.h"

// AActor* ASoulDrive2GameModeBase::ChoosePlayerStart_Implementation(AController* Player)
// {
// 	AActor* result;
// 	if (playerStartArray.Num() == 0)
// 	{
// 		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), playerStartArray);
// 	}
// 	for (AActor *PStart : playerStartArray)
// 	{
// 		if (PStart->ActorHasTag(FName("player1"))
// 			&& Player->GetName().Compare(FString("SharedPlayerController_C_0")))
// 		{
// 			return PStart;
// 		}
// 		else if (PStart->ActorHasTag(FName("player2"))
// 			&& Player->GetName().Compare(FString("SharedPlayerController_C_1")))
// 		{
// 			return PStart;
// 		}
// 	}
// 	result = playerStartArray.Pop(true);
// 
// 	return result;
// }
// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include "SDNetPlayerProxy.h"
#include "SDNetPlayerControllerProxy.h"
#include "SoulDrive2GameModeBase.h"
#include "List.h"


AActor* ASoulDrive2GameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	AActor* result = nullptr;
	if (playerStartArray.Num() == 0)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), playerStartArray);
	}
	for (AActor *PStart : playerStartArray)
	{
		if (PStart->ActorHasTag(FName("player1"))
			&& Player->GetName().Compare(FString("SharedPlayerController_C_0")))
		{
			return PStart;
		}
		else if (PStart->ActorHasTag(FName("player2"))
			&& Player->GetName().Compare(FString("SharedPlayerController_C_1")))
		{
			return PStart;
		}
	}
	if (playerStartArray.Num() > 0)
	{
		result = playerStartArray.Pop(true);
	}

	return result;
}


void ASoulDrive2GameModeBase::GenerateMapData(UPARAM(ref) TArray<int> &TileList, FMapGenerationParams Params)
{
	int RandomIndex = 0;
	TArray<int> availableIndices;
	TileList.SetNum(Params.TileCountX * Params.TileCountY);
	availableIndices.SetNum(TileList.Num());

	for (int i = 0; i < TileList.Num(); i++)
	{
		TileList[i] = Params.ActorIds[FMath::RandRange(0, Params.ActorIds.Num() - 1)];
		availableIndices[i] = i;
	}

	// Write over locations in tile list. Remove indices from availableIndices to avoid collisions
	for (TPair<int, int> KeyFrequencyPair : Params.ExactlyNIds)
	{
		for (int j = 0; j < KeyFrequencyPair.Value; j++)
		{
			RandomIndex = FMath::RandRange(0, availableIndices.Num() - 1);
			TileList[availableIndices[RandomIndex]] = KeyFrequencyPair.Key;
			availableIndices.RemoveAt(RandomIndex);
		}
	}
}

void ASoulDrive2GameModeBase::SpawnPlayerCharacter(APlayerController* Player) 
{
	AActor* SpawnAt = ChoosePlayerStart_Implementation(Player);
	APawn* SpawnedCharacter;
	ASDNetPlayerControllerProxy *ProxyController = dynamic_cast<ASDNetPlayerControllerProxy *>(Player);

	FActorSpawnParameters SpawnParams;
	if (SpawnAt != nullptr)
	{
		SpawnedCharacter = GetWorld()->SpawnActor<ASDNetPlayerProxy>(SpawnAt->GetActorLocation(), SpawnAt->GetActorRotation(), SpawnParams);
	} 
	else
	{
		SpawnedCharacter = GetWorld()->SpawnActor<ASDNetPlayerProxy>(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnParams);
	}

	if (SpawnedCharacter != nullptr)
	{
		if (ProxyController != nullptr)
		{
			ProxyController->SetControllerInputModeGame();
		}
		Player->Possess(SpawnedCharacter);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn character!"));
	}
}

void ASoulDrive2GameModeBase::BuildLevelMulticast_Implementation(FRandomStream Stream)
{
	//GenerateMapData()
}

bool ASoulDrive2GameModeBase::BuildLevelMulticast_Validate(FRandomStream Stream)
{
	return true;
}

int32 ASoulDrive2GameModeBase::GetListKeyByIndex(TDoubleLinkedList<int32> &List, int32 Index)
{
	TDoubleLinkedList<int32>::TIterator *Itr = new TDoubleLinkedList<int32>::TIterator(List.GetHead());

	int32 result = -1;
	int32 counter = 0;
	for (int32 value : List)
	{
		if (++counter > Index)
		{
			result = value;
			List.RemoveNode(value);
			break;
		}
	}
	return result;
}

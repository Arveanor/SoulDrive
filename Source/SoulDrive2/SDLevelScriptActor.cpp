// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "Net/UnrealNetwork.h"
#include "SDLevelScriptActor.h"




ASDLevelScriptActor::ASDLevelScriptActor(const class FObjectInitializer& FOI)
	: Super(FOI)
{
	bReplicates = true;
}

void ASDLevelScriptActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASDLevelScriptActor, Stream);
}

void ASDLevelScriptActor::GenerateLevelFromStream()
{
	FActorSpawnParameters SpawnParams;
	ASDIsmLevelBase *levelbase = GetWorld()->SpawnActor<ASDIsmLevelBase>(IsmParentBP, FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f), SpawnParams);
//	GenerateMapData();
	levelbase->LoadIsm(TileList, Params.TileCountX, Params.TileCountY);
	ASDCommonUtilities::AllControllersPostLevelLoad(this);
}
// 
// void ASDLevelScriptActor::GenerateMapData()
// {
// 	int RandomIndex = 0;
// 	TArray<int> availableIndices;
// 	TileList.SetNum(Params.TileCountX * Params.TileCountY);
// 	availableIndices.SetNum(TileList.Num());
// 
// 	for (int i = 0; i < TileList.Num(); i++)
// 	{
// 		TileList[i] = Params.ActorIds[FMath::RandRange(0, Params.ActorIds.Num() - 1)];
// 		availableIndices[i] = i;
// 	}
// 
// 	// Write over locations in tile list. Remove indices from availableIndices to avoid collisions
// 	for (TPair<int, int> KeyFrequencyPair : Params.ExactlyNIds)
// 	{
// 		for (int j = 0; j < KeyFrequencyPair.Value; j++)
// 		{
// 			RandomIndex = FMath::RandRange(0, availableIndices.Num() - 1);
// 			TileList[availableIndices[RandomIndex]] = KeyFrequencyPair.Key;
// 			availableIndices.RemoveAt(RandomIndex);
// 		}
// 	}
// }

// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDPlayerState.h"




ASDPlayerState::ASDPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DebugKey = 3;
}

void ASDPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASDPlayerState, DebugKey);
	DOREPLIFETIME(ASDPlayerState, HostAddr);
}
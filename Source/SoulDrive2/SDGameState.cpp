// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDGameState.h"




ASDGameState::ASDGameState()
{
	IsMapGenerationCompleted = false;
}

void ASDGameState::SetIsMapGenerationCompleted(bool generationFlag)
{
	IsMapGenerationCompleted = generationFlag;
}

bool ASDGameState::GetIsMapGenerationCompleted()
{
	return IsMapGenerationCompleted;
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDBaseAoE.h"
#include "SDSunBurstSpell.h"




void ASDSunBurstSpell::CastSpell(FVector target)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	ASDBaseAoE *SpawnedAoE = GetWorld()->SpawnActor<ASDBaseAoE>(target, FRotator(0.f, 0.f, 0.f), SpawnInfo);
	SpawnedAoE->SetParentSpell(this);
}

void ASDSunBurstSpell::BeginPlay()
{
	Super::BeginPlay();
}

void ASDSunBurstSpell::HandleTarget(AActor *Target, bool IsAlly)
{
	const FDamageEvent DamageEvent;
	UE_LOG(LogTemp, Warning, TEXT("Sunburst handle target happening"));
	if (GetInstigator() != nullptr && !IsAlly)
	{
		Target->TakeDamage(10.0f, DamageEvent, GetInstigator()->GetController(), this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("instigator was null"));
	}
}

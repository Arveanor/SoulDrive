// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "Net/UnrealNetwork.h"
#include "SDBaseSpell.h"


// Sets default values
ASDBaseSpell::ASDBaseSpell(const class FObjectInitializer& FOI)
	: Super(FOI)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
}

void ASDBaseSpell::Init(AController* OwnedBy)
{
	Caster = OwnedBy;
	CasterCharacter = dynamic_cast<ACharacter *>(Caster->GetPawn());
	spellTestInt = 112;
}

void ASDBaseSpell::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ASDBaseSpell, Caster);
	DOREPLIFETIME(ASDBaseSpell, CasterCharacter);
	DOREPLIFETIME(ASDBaseSpell, spellTestInt);
}

// Called when the game starts or when spawned
void ASDBaseSpell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASDBaseSpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASDBaseSpell::CastSpell(FVector target)
{
	UE_LOG(LogTemp, Warning, TEXT("SpellSlot0 being cast in base class"));
}

void ASDBaseSpell::HandleTarget(AActor *Target, bool IsAlly)
{
	UE_LOG(LogTemp, Warning, TEXT("HandleTarget being cast in base class"));
}

AController* ASDBaseSpell::GetCaster()
{
	return Caster;
}

ACharacter* ASDBaseSpell::GetCasterCharacter()
{
	return CasterCharacter;
}

void ASDBaseSpell::SetTeamId(int InTeamId)
{
	TeamId = InTeamId;
}

int ASDBaseSpell::GetTeamId()
{
	return TeamId;
}

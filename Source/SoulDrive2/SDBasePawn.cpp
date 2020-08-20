// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDPlayerState.h"
#include "SDNetPlayerControllerProxy.h"
#include "SDNetPlayerProxy.h"
#include "SDBasePawn.h"


class ASDNetPlayerControllerProxy;

// Sets default values
ASDBasePawn::ASDBasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ASDBasePawn::SetPlayerID(int id)
{
	PlayerId = id;
}

int ASDBasePawn::GetPlayerID()
{
	return PlayerId;
}

// Called when the game starts or when spawned
void ASDBasePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASDBasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASDBasePawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASDBasePawn, MaxHp);
	DOREPLIFETIME(ASDBasePawn, CurrentHp);
	DOREPLIFETIME(ASDBasePawn, MaxMana);
	DOREPLIFETIME(ASDBasePawn, CurrentMana);
}

// Called to bind functionality to input
void ASDBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASDBasePawn::CastSpell(FName SpellName, FVector AimedAt)
{

}

float ASDBasePawn::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	/*
	** NetOwner here is the same as this but in NetPlayerPawn::BeginPlay it is correctly showing SDNetPlayerProxyController as the owner
	** Strong suspicion is that this is either a replication problem or ...
	*/
	CurrentHp -= Damage;
	const AActor* MyNetOwner = this->GetOwner();
	const ASDNetPlayerControllerProxy* ProxyController = dynamic_cast<const ASDNetPlayerControllerProxy*>(MyNetOwner);
	if (ProxyController != nullptr)
	{
		ASDPlayerState* OwningPlayerState = dynamic_cast<ASDPlayerState*>(ProxyController->PlayerState);
		OwningPlayerState->DebugKey = 88;
	}
	return Damage;
}

int ASDBasePawn::GetTeamId_Implementation()
{
	return TeamId;
}

void ASDBasePawn::SetTimeId(int InTeamId)
{
	TeamId = InTeamId;
}

float ASDBasePawn::GetHpRatio()
{
	return CurrentHp / MaxHp;
}

float ASDBasePawn::GetManaRatio()
{
	return CurrentMana / MaxMana;
}

float ASDBasePawn::GetCurrentMana()
{
	return CurrentMana;
}

void ASDBasePawn::BurnMana(int amount)
{
	CurrentMana -= amount;
}

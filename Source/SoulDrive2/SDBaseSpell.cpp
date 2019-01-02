// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "Net/UnrealNetwork.h"
#include "SDNetPlayerPawn.h"
#include "SDNetPlayerControllerProxy.h"
#include "SDBaseSpell.h"


// Sets default values
ASDBaseSpell::ASDBaseSpell(const class FObjectInitializer& FOI)
	: Super(FOI)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	OnCooldown = false;
	CooldownLength = 0.0f;
}

void ASDBaseSpell::Init(AController* OwnedBy)
{
	Caster = OwnedBy;
	CasterCharacter = dynamic_cast<ACharacter *>(Caster->GetPawn());
}

void ASDBaseSpell::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ASDBaseSpell, Caster);
	DOREPLIFETIME(ASDBaseSpell, CasterCharacter);
	DOREPLIFETIME(ASDBaseSpell, CooldownLength);
	DOREPLIFETIME(ASDBaseSpell, OnCooldown);
	DOREPLIFETIME(ASDBaseSpell, TimerHandler);
}

// Called when the game starts or when spawned
void ASDBaseSpell::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASDBaseSpell::CooldownTimerCallback()
{
	OnCooldown = false;
}

bool ASDBaseSpell::PreCast()
{
	if (OnCooldown)
		return false;

	ASDNetPlayerPawn* SDCasterCharacter = dynamic_cast<ASDNetPlayerPawn *> (CasterCharacter);
	ASDNetPlayerControllerProxy* SDCaster = dynamic_cast<ASDNetPlayerControllerProxy *> (Caster);
	if (SDCaster != nullptr)
		if (SDCaster->GetMainWeapon()->WeaponType != WeaponType)
			return false;

	if (ManaCost > 0)
	{
		if (SDCasterCharacter != nullptr)
			if (SDCasterCharacter->GetCurrentMana() - ManaCost < 0)
				return false;
			else
				SDCasterCharacter->BurnMana(ManaCost);
	}
	if (SDCasterCharacter != nullptr)
	{
		SDCasterCharacter->SetIsCasting(true);
	}
	Caster->StopMovement();
	if (CooldownLength > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandler, this, &ASDBaseSpell::CooldownTimerCallback, CooldownLength, false, -1.0f);
		OnCooldown = true;
	}
	return true;
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

bool ASDBaseSpell::isOnCooldown()
{
	return OnCooldown;
}

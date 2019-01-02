// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDSunBurstSpell.h"
#include "SDBaseAICharacter.h"


// Sets default values
ASDBaseAICharacter::ASDBaseAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = ASDBaseAIController::StaticClass();
	SunburstCooldown = 1.5f;
	CurrentHp = 19;
	MaxHp = 19;
	this->TeamId = 7;
}

void ASDBaseAICharacter::CastSpell(FName SpellName, FVector AimedAt)
{
	Sunburst->CastSpell(AimedAt);
	GetWorld()->GetTimerManager().SetTimer(TimerHandler, this, &ASDBaseAICharacter::CooldownFinished, SunburstCooldown, false, SunburstCooldown);
	IsSunburstOnCooldown = true;
}

void ASDBaseAICharacter::CooldownFinished()
{
	IsSunburstOnCooldown = false;
}

bool ASDBaseAICharacter::GetIsSpellOnCooldown()
{
	return IsSunburstOnCooldown;
}

// Called when the game starts or when spawned
void ASDBaseAICharacter::BeginPlay()
{
	Super::BeginPlay();
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = this;
	Sunburst = GetWorld()->SpawnActor<ASDSunBurstSpell>(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnInfo);
}

float ASDBaseAICharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	CurrentHp -= Damage;
	if (CurrentHp <= 0)
	{
		this->Destroy();
	}
	return 0.0f;
}

// Called every frame
void ASDBaseAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASDBaseAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDGameInstance.h"
#include "SDNetPlayerProxy.h"
#include "SDNetPlayerControllerProxy.h"
#include "SDFireBoltSpell.h"
#include "SDCelestialFragmentSpell.h"
#include "SDSunBurstSpell.h"
#include "SDSlash.h"
#include "SDRangedAttack.h"
#include "SDPortal.h"


// Sets default values
ASDNetPlayerProxy::ASDNetPlayerProxy()
{
	bReplicates = true;
	SetActorEnableCollision(true);
//	GetCapsuleComponent()->SetCollisionProfileName(FName("NoCollision"));
	PrimaryActorTick.bCanEverTick = true;

// 	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
// 	RootComponent = SceneRoot;

	// Create a camera boom...
	MainCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	MainCameraBoom->SetupAttachment(RootComponent);
	MainCameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	MainCameraBoom->TargetArmLength = 2800.f;
	MainCameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	MainCameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	PlayerCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	PlayerCameraComponent->SetupAttachment(MainCameraBoom, USpringArmComponent::SocketName);
	PlayerCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	LerpTarget = FVector(0.0f, 0.0f, 0.0f);

//	this->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ASDNetPlayerProxy::OnOverlapBegin);
// 	USDGameInstance *GameInstance = dynamic_cast<USDGameInstance *>(GetGameInstance());
// 	GameInstance->OnTestDelegate.AddDynamic(this, &ASDNetPlayerProxy::OnFinalCall);
	
}

// Called when the game starts or when spawned
void ASDNetPlayerProxy::BeginPlay()
{
	Super::BeginPlay();
	
	if (SDConstants::CheatMode)
	{
		FActorSpawnParameters ItemSpawnInfo;
		ASDBaseEquipment *DevItem;
		DevItem = GetWorld()->SpawnActor<ASDBaseEquipment>(GetActorLocation(), FRotator(0.f, 0.f, 0.f), ItemSpawnInfo);
		DevItem->ItemName = FName(TEXT("Universal key"));
	}

	if (SDConstants::CheatMode && HasAuthority())
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Instigator = this;
		SpellSlot0 = GetWorld()->SpawnActor<ASDCelestialFragmentSpell>(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnInfo);
		SpellSlot1 = GetWorld()->SpawnActor<ASDFireBoltSpell>(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnInfo);
		SpellSlot2 = GetWorld()->SpawnActor<ASDSlash>(FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f), SpawnInfo);
		SpellSlot3 = GetWorld()->SpawnActor<ASDRangedAttack>(FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f), SpawnInfo);
	}
}

bool ASDNetPlayerProxy::SetPortalTarget_Validate(const FString& InURL, ASDPortal* Portal)
{
	return true;
}

void ASDNetPlayerProxy::SetPortalTarget_Implementation(const FString& InURL, ASDPortal* Portal)
{
	Portal->ExistingURL = InURL;
}

void ASDNetPlayerProxy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASDNetPlayerProxy, ServerController);
	DOREPLIFETIME(ASDNetPlayerProxy, ServerCharacter);
	DOREPLIFETIME(ASDNetPlayerProxy, SpellSlot0);
	DOREPLIFETIME(ASDNetPlayerProxy, SpellSlot1);
	DOREPLIFETIME(ASDNetPlayerProxy, SpellSlot2);
	DOREPLIFETIME(ASDNetPlayerProxy, SpellSlot3);
	DOREPLIFETIME(ASDNetPlayerProxy, IsMoving);

}

// Called every frame
void ASDNetPlayerProxy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ASDNetPlayerProxy::TravelToLevel_Validate(FName LevelToLoad)
{
	return true;
}


void ASDNetPlayerProxy::TravelToLevel_Implementation(FName LevelToLoad)
{
	ASDPortal* InteractionPortal = dynamic_cast<ASDPortal*>(InteractionTarget);

 	ASDNetPlayerControllerProxy* ProxyController = dynamic_cast<ASDNetPlayerControllerProxy*>(GetController());

	if (HasAuthority())
	{
		if (InteractionPortal != nullptr && !InteractionPortal->ExistingURL.IsEmpty())
		{
			if (ProxyController != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Target URL is %s"), *InteractionPortal->ExistingURL);
//				GetController()->ClientTravel(InteractionPortal->ExistingURL, ETravelType::TRAVEL_Absolute);
				GetWorld()->ServerTravel(InteractionPortal->ExistingURL, false, false);
			}
		}
		if (InteractionPortal != nullptr)
		{
			if (GetController()->GetNetConnection() != nullptr)
				SetPortalTarget(ProxyController->GetNetConnection()->URL.ToString(), InteractionPortal);
		}
		ProxyController->ClientTravel(LevelToLoad.ToString(), ETravelType::TRAVEL_Relative, true);
	}
}


void ASDNetPlayerProxy::OnFinalCall()
{
}


float ASDNetPlayerProxy::GetDisplayHealth()
{
	if (ServerCharacter != nullptr)
	{
		return ServerCharacter->GetHpRatio();
	}
	else
	{
		return 0.0f;
	}
}

float ASDNetPlayerProxy::GetDisplayMana()
{
	if (ServerCharacter != nullptr)
	{
		return ServerCharacter->GetManaRatio();
	}
	else
	{
		return 0.0f;
	}
}

void ASDNetPlayerProxy::SetPlayerId(uint8 Id)
{
	PlayerId = Id;
}

uint8 ASDNetPlayerProxy::GetPlayerId()
{
	return PlayerId;
}

void ASDNetPlayerProxy::SetIsCasting(bool _isCasting)
{
	isCasting = _isCasting;
}

bool ASDNetPlayerProxy::SetInteractionTarget_Validate(AActor* Target)
{
	return true;
}

void ASDNetPlayerProxy::SetInteractionTarget_Implementation(AActor* Target)
{
	InteractionTarget = Target;
}

AActor* ASDNetPlayerProxy::GetInteractionTarget()
{
	return InteractionTarget;
}

bool ASDNetPlayerProxy::IsCasting()
{
	return isCasting;
}

ASDNetPlayerController * ASDNetPlayerProxy::GetServerController()
{
	return ServerController;
}

ASDNetPlayerPawn * ASDNetPlayerProxy::GetServerCharacter()
{
	return ServerCharacter;
}

void ASDNetPlayerProxy::SetLerpTarget_Implementation(FVector target)
{
	this->SetActorLocation(FMath::Lerp(this->GetActorLocation(), target, 0.01));
}

bool ASDNetPlayerProxy::SetLerpTarget_Validate(FVector target)
{
	return true;
}

void ASDNetPlayerProxy::SetServerController(ASDNetPlayerController *NetControllerS)
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("HON HON"));
	}
	ServerController = NetControllerS;
	ServerCharacter = dynamic_cast<ASDNetPlayerPawn *>(NetControllerS->GetPawn());
}

ASDBaseSpell* ASDNetPlayerProxy::GetSpellSlot(uint8 Slot)
{
	switch (Slot)
	{
	case 0:
		return SpellSlot0;
	case 1:
		return SpellSlot1;
	case 2:
		return SpellSlot2;
	case 3:
		return SpellSlot3;
	default:
		return SpellSlot0;
	}
}

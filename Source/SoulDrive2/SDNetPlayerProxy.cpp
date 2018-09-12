// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDGameInstance.h"
#include "SDNetPlayerProxy.h"


// Sets default values
ASDNetPlayerProxy::ASDNetPlayerProxy(const class FObjectInitializer& FOI)
	: Super (FOI)
{
	bReplicates = true;
	SetActorEnableCollision(false);
//	GetCapsuleComponent()->SetCollisionProfileName(FName("NoCollision"));
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;

	// Create a camera boom...
	MainCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	MainCameraBoom->SetupAttachment(RootComponent);
	MainCameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	MainCameraBoom->TargetArmLength = 2800.f;
	MainCameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
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
	if (AActor::HasAuthority())
	{
		if (NetCharacterClass != nullptr && NetControllerClass != nullptr)
		{
			FActorSpawnParameters SpawnParams;
			FVector SpawnLocation = GetActorLocation();
			UE_LOG(LogTemp, Warning, TEXT("Attempting to spawn Server Character at %s"), *SpawnLocation.ToString());
			ServerCharacter = GetWorld()->SpawnActor<ASDNetPlayerPawn>(NetCharacterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			if (ServerCharacter != nullptr)
			{
				ServerController = GetWorld()->SpawnActor<ASDNetPlayerController>(NetControllerClass, SpawnParams);
				ServerController->Possess(ServerCharacter);
				SetServerController(ServerController);
				if (ServerController != nullptr)
				{
					UE_LOG(LogTemp, Warning, TEXT("Successfully created server controller!"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Could not create server controller!"));
				}
			}
		}
	}

	if (SDConstants::CheatMode)
	{
		FActorSpawnParameters ItemSpawnInfo;
		ASDBaseEquipment *DevItem;
		DevItem = GetWorld()->SpawnActor<ASDBaseEquipment>(GetActorLocation(), FRotator(0.f, 0.f, 0.f), ItemSpawnInfo);
		DevItem->ItemName = FName(TEXT("Universal key"));
	}
}

void ASDNetPlayerProxy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ASDNetPlayerProxy, ServerController);
	DOREPLIFETIME(ASDNetPlayerProxy, ServerCharacter);
}

// Called every frame
void ASDNetPlayerProxy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
// 	if (GetNetMode() == ENetMode::NM_ListenServer)
// 	{
		if (ServerCharacter != nullptr)
		{
			FVector ServerClientDistance = this->GetActorLocation() - ServerCharacter->GetActorLocation();
			if (ServerClientDistance.Size() > 50.0f)
			{
				ServerCharacter->IsMoving = true;
				this->SetActorLocation(FMath::Lerp(this->GetActorLocation(), ServerCharacter->GetActorLocation(), 0.01));
				this->SetLerpTarget(ServerCharacter->GetActorLocation());
			}
			else
			{
				ServerCharacter->IsMoving = false;
			}
		}
/*	}*/
}

// Called to bind functionality to input
// void ASDNetPlayerProxy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
// {
// 	Super::SetupPlayerInputComponent(PlayerInputComponent);
// 
// }

void ASDNetPlayerProxy::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("proxy pawn overlap event!"));
}

void ASDNetPlayerProxy::OnFinalCall()
{
}

bool ASDNetPlayerProxy::DropItem(ASDBaseEquipment * DroppedItem)
{
	return ServerCharacter->DropItem(DroppedItem);
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

void ASDNetPlayerProxy::SetServerController_Implementation(ASDNetPlayerController *NetControllerS)
{
	ServerController = NetControllerS;
}

bool ASDNetPlayerProxy::SetServerController_Validate(ASDNetPlayerController *NetControllerS)
{
	return true;
}
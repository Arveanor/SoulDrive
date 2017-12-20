// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDNetPlayerProxy.h"


// Sets default values
ASDNetPlayerProxy::ASDNetPlayerProxy()
{
	bReplicates = true;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

			ServerCharacter = GetWorld()->SpawnActor<ASDNetPlayerPawn>(NetCharacterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
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

// Called every frame
void ASDNetPlayerProxy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ServerCharacter != nullptr)
	{
		FVector ServerClientDistance = this->GetActorLocation() - ServerCharacter->GetActorLocation();
		if (ServerClientDistance.Size() > 50.0f)
		{
			this->SetActorLocation(FMath::Lerp(this->GetActorLocation(), ServerCharacter->GetActorLocation(), 0.01));
		}
	}
}

// Called to bind functionality to input
void ASDNetPlayerProxy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

ASDNetPlayerController * ASDNetPlayerProxy::GetServerController()
{
	return ServerController;
}

void ASDNetPlayerProxy::SetServerController_Implementation(ASDNetPlayerController *NetControllerS)
{
	ServerController = NetControllerS;
}

bool ASDNetPlayerProxy::SetServerController_Validate(ASDNetPlayerController *NetControllerS)
{
	return true;
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDNetPlayerControllerProxy.h"


ASDNetPlayerControllerProxy::ASDNetPlayerControllerProxy()
{
	bMoveToLocation = false;
	bShowMouseCursor = true;
}

// void ASDNetPlayerControllerProxy::MoveToLocation()
// {
// 	 FHitResult Hit;
// 	 GetHitResultUnderCursor(ECC_Visibility, false, Hit);
// 	 UE_LOG(LogTemp, Warning, TEXT("Top level of MoveToLocation"));
// 	 if (ServerController == nullptr && PlayerProxy != nullptr)
// 	 {
// 	 	UE_LOG(LogTemp, Warning, TEXT("PlayerProxy is not null"));
// 	 	ServerController = PlayerProxy->GetServerController();
// 	 }
// 	 else
// 	 {
// 	 	UE_LOG(LogTemp, Warning, TEXT("PlayerProxy is null"));
// 	 }
// 	 if (ServerController == nullptr)
// 	 {
// 		 UE_LOG(LogTemp, Warning, TEXT("ServerController is null"));
// 	 }
// 	 if (Hit.bBlockingHit && ServerController != nullptr)
// 	 {
// 		UE_LOG(LogTemp, Warning, TEXT("Attempting to move the player character"));
// 		ServerController->MoveToLocation(Hit.Location);
// 	}
// }

void ASDNetPlayerControllerProxy::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	EnableInput(this);
	if (bMoveToLocation)
	{
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);
		if (Hit.bBlockingHit)
		{
			MoveToLocation(Hit);
		}
	}
}

void ASDNetPlayerControllerProxy::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ASDNetPlayerControllerProxy::OnMovementKeyPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ASDNetPlayerControllerProxy::OnMovementKeyReleased);
}

void ASDNetPlayerControllerProxy::BeginPlay()
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client side Net Player Controller Proxy BeginPlay"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Server side Net Player Controller Proxy BeginPlay"));
	}
	PlayerProxy = (ASDNetPlayerProxy *)GetPawn();
	if (PlayerProxy != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("successfully grabbed player proxy instance!"));
		ServerController = PlayerProxy->GetServerController();
		if (ServerController != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("successfully grabbed player server controller!"));
		}
	}
	else if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Server has no player proxy instance!"));
	}
}

void ASDNetPlayerControllerProxy::MoveToLocation_Implementation(FHitResult HitResult)
{
	if (ServerController == nullptr && PlayerProxy != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerProxy is not null"));
		ServerController = PlayerProxy->GetServerController();
	}
	else
	{
		PlayerProxy = (ASDNetPlayerProxy *)GetPawn();
		ServerController = PlayerProxy->GetServerController();
	}
	if (!HitResult.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("not a blocking hit :("));
	}
	if (HitResult.bBlockingHit && ServerController != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to move the player character"));
		ServerController->MoveToLocation(HitResult.Location);
	}
}

bool ASDNetPlayerControllerProxy::MoveToLocation_Validate(FHitResult HitResult)
{
	return true;
}

void ASDNetPlayerControllerProxy::OnMovementKeyPressed()
{
	bMoveToLocation = true;
}

void ASDNetPlayerControllerProxy::OnMovementKeyReleased()
{
	bMoveToLocation = false;
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDPlayerController.h"




void ASDPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	EnableInput(this);

	if (IsInputKeyDown(EKeys::Y))
	{
		UE_LOG(LogTemp, Warning, TEXT("Y was pressed!"));
	}

	if (bMoveToCursor)
	{
		MoveToCursor();
	}
}

void ASDPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ASDPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ASDPlayerController::OnSetDestinationReleased);

}

void ASDPlayerController::OnSetDestinationPressed()
{
	bMoveToCursor = true;
}

void ASDPlayerController::OnSetDestinationReleased()
{
	bMoveToCursor = false;
}

void ASDPlayerController::MoveToCursor()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		NavSys->SimpleMoveToLocation(this, Hit.ImpactPoint);
	}
}

ASDPlayerController::ASDPlayerController()
{
	bMoveToCursor = false;
	bShowMouseCursor = true;
}

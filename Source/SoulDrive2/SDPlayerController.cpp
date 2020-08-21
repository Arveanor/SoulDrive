// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "NavigationSystem.h"
#include "SDPlayerController.h"



ASDPlayerController::ASDPlayerController()
{
	bMoveToCursor = false;
	bShowMouseCursor = true;
	MenuCanBeOpened = true;
	OverwritableAction = SDConstants::HotKeyOverrides::NO_ACTION_WRITABLE;
}

void ASDPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (wPlayerGameMenu)
	{
		PlayerGameMenu = CreateWidget<UUserWidget>(this, wPlayerGameMenu);
		if (PlayerGameMenu != nullptr)
		{
			KeyRebindInput.SetWidgetToFocus(PlayerGameMenu->TakeWidget());
		}
	}
}

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

	InputComponent->BindAction("DebugAction", IE_Pressed, this, &ASDPlayerController::OnDebugActionPressed);
	InputComponent->BindAction("DebugAction", IE_Released, this, &ASDPlayerController::OnDebugActionReleased);

	InputComponent->BindAction("LaunchMenu", IE_Pressed, this, &ASDPlayerController::OnLaunchPlayerMenu);
	InputComponent->BindAction("LaunchMenu", IE_Released, this, &ASDPlayerController::OnClosePlayerMenu);

}

void ASDPlayerController::OnSetDestinationPressed()
{
	bMoveToCursor = true;
}

void ASDPlayerController::OnSetDestinationReleased()
{
	bMoveToCursor = false;
}

void ASDPlayerController::OnDebugActionPressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Used the debug action! %d"), PlayerInput->ActionMappings.Num()));
}

void ASDPlayerController::OnDebugActionReleased()
{
	//ASoulDriveGameModeBase* CurrentGameMode = (ASoulDriveGameModeBase *)GetWorld()->GetAuthGameMode();
	//CurrentGameMode->LoadSpell(SpellSlot1);
}

void ASDPlayerController::MoveToCursor()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		RequestMoveToLocation(this, Hit.ImpactPoint);
	}
}

void ASDPlayerController::OverrideHotkey(const FKey Key)
{
	FName Action;
	UE_LOG(LogTemp, Warning, TEXT("OverrideHotkey entered"));
	UE_LOG(LogTemp, Warning, TEXT("Action = %d"), OverwritableAction);

	if (OverwritableAction == 1)
	{
		Action = FName(SDConstants::DEBUG_ACTION_COMMAND);
		DebugActionAssignedKey = Key.GetFName().ToString();
	}
	else
	{
		Action = FName("NONE");
	}

	if (Action.Compare(FName(SDConstants::DEBUG_ACTION_COMMAND)) == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Valid command recognized"));
		for (FInputActionKeyMapping Mapping : PlayerInput->ActionMappings)
		{
			if (Mapping.ActionName.IsEqual(Action))
			{
				PlayerInput->ActionMappings.Remove(Mapping);
			}
		}
		FInputActionKeyMapping KeyMapping(Action, Key, false, false, false, false);
		PlayerInput->AddActionMapping(KeyMapping);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("no valid command recognized"));
	}

	OverwritableAction = SDConstants::HotKeyOverrides::NO_ACTION_WRITABLE;
}

void ASDPlayerController::OnLaunchPlayerMenu()
{
	if (PlayerGameMenu != nullptr)
	{
		if (!PlayerGameMenu->IsInViewport() && MenuCanBeOpened)
		{
			SetMenuCanBeOpened(false);
			PlayerGameMenu->AddToViewport();
			SetInputMode(KeyRebindInput);
		}
		else
		{
			PlayerGameMenu->RemoveFromViewport();
			SetInputMode(StandardInput);
		}
	}
}

void ASDPlayerController::OnClosePlayerMenu()
{
	SetMenuCanBeOpened(true);
}

void ASDPlayerController::RequestMoveToLocation_Implementation(AController *TargetController, const FVector &Goal)
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	NavSys->SimpleMoveToLocation(TargetController, Goal);
}

bool ASDPlayerController::RequestMoveToLocation_Validate(AController * TargetController, const FVector & Goal)
{
	return true;
}

void ASDPlayerController::SetMenuCanBeOpened(bool NewValue)
{
	MenuCanBeOpened = NewValue;
}
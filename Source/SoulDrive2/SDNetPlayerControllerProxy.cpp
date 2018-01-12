// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDNetPlayerControllerProxy.h"


ASDNetPlayerControllerProxy::ASDNetPlayerControllerProxy()
{
	bMoveToLocation = false;
	bShowMouseCursor = true;
	HotkeyMenuCanBeOpened = true;
	OverwritableAction = SDConstants::HotKeyOverrides::NO_ACTION_WRITABLE;
}

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

	InputComponent->BindAction("DebugAction", IE_Pressed, this, &ASDNetPlayerControllerProxy::OnDebugActionPressed);
	InputComponent->BindAction("DebugAction", IE_Released, this, &ASDNetPlayerControllerProxy::OnDebugActionReleased);

	InputComponent->BindAction("LaunchMenu", IE_Pressed, this, &ASDNetPlayerControllerProxy::OnLaunchPlayerMenu);
	InputComponent->BindAction("LaunchMenu", IE_Released, this, &ASDNetPlayerControllerProxy::OnClosePlayerMenu);

	InputComponent->BindAction("SpellSlot0", IE_Pressed, this, &ASDNetPlayerControllerProxy::OnSpellSlot0Pressed);
	InputComponent->BindAction("SpellSlot0", IE_Released, this, &ASDNetPlayerControllerProxy::OnSpellSlot0Released);

	InputComponent->BindAction("LaunchMpMenu", IE_Pressed, this, &ASDNetPlayerControllerProxy::OnLaunchMpMenu);
	InputComponent->BindAction("LaunchMpMenu", IE_Released, this, &ASDNetPlayerControllerProxy::OnCloseMpMenu);
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
	
	if (wPlayerGameMenu)
	{
		PlayerGameMenu = CreateWidget<UUserWidget>(this, wPlayerGameMenu);
		if (PlayerGameMenu != nullptr)
		{
			KeyRebindInput.SetWidgetToFocus(PlayerGameMenu->TakeWidget());
		}
	}

	if (wMpMenu)
	{
		MpMenu = CreateWidget<UUserWidget>(this, wMpMenu);
	}

	if (SDConstants::CheatMode)
	{
		FActorSpawnParameters SpawnInfo;
		SpellSlot0 = GetWorld()->SpawnActor<ASDCheatSpell>(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnInfo);
		ASDCheatSpell* ChildRef = dynamic_cast<ASDCheatSpell*>(SpellSlot0);

		if (ServerController == nullptr && PlayerProxy != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Getting ServerController from Pawn Proxy"));
			ServerController = PlayerProxy->GetServerController();
		}
		if (ChildRef != nullptr && ServerController != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Initializing SpellSlot0"));
			ChildRef->Init(ServerController->GetPawn());
		}
		else
		{
			if (ChildRef == nullptr) UE_LOG(LogTemp, Warning, TEXT("dynamic cast failed!"));
			if (ServerController == nullptr) UE_LOG(LogTemp, Warning, TEXT("Could not get controller from proxy"));
		}
	}
}

void ASDNetPlayerControllerProxy::OnDebugActionPressed()
{

}

void ASDNetPlayerControllerProxy::OnDebugActionReleased()
{

}

void ASDNetPlayerControllerProxy::OnSpellSlot0Pressed()
{
	if (SpellSlot0 != nullptr)
	{
		ASDCheatSpell *AsCheatSpell = dynamic_cast<ASDCheatSpell *>(SpellSlot0);
		if (ServerController == nullptr)
		{
			ServerController = PlayerProxy->GetServerController();
		}
		AsCheatSpell->Init(ServerController->GetPawn());
		SpellSlot0->CastSpell(FVector(0.0f, 0.0f, 0.0f));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SpellSlot0 not being cast"));
	}
}

void ASDNetPlayerControllerProxy::OnSpellSlot0Released()
{

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

void ASDNetPlayerControllerProxy::OnLaunchPlayerMenu()
{
	if (PlayerGameMenu != nullptr)
	{
		if (!PlayerGameMenu->IsInViewport() && HotkeyMenuCanBeOpened)
		{
			SetHotkeyMenuCanBeOpened(false);
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

void ASDNetPlayerControllerProxy::OnLaunchMpMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("OnLaunchMpMenu entered"));
	if (MpMenu != nullptr)
	{
		if (!MpMenu->IsInViewport() && MpMenuCanBeOpened)
		{
			SetMpMenuCanBeOpened(false);
			MpMenu->AddToViewport();
			SetInputMode(KeyRebindInput);
		}
		else
		{
			MpMenu->RemoveFromViewport();
			SetInputMode(StandardInput);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MpMenu is null"));
	}
}

void ASDNetPlayerControllerProxy::SetHotkeyMenuCanBeOpened(bool NewValue)
{
	HotkeyMenuCanBeOpened = NewValue;
}

void ASDNetPlayerControllerProxy::SetMpMenuCanBeOpened(bool NewValue)
{
	MpMenuCanBeOpened = NewValue;
}

void ASDNetPlayerControllerProxy::OverrideHotkey(const FKey Key)
{
	FName Action;
	UE_LOG(LogTemp, Warning, TEXT("OverrideHotkey entered"));
	UE_LOG(LogTemp, Warning, TEXT("Action = %d"), OverwritableAction);

	if (OverwritableAction == 1)
	{
		Action = FName(SDConstants::DEBUG_ACTION_COMMAND);
		DebugActionAssignedKey = Key.GetFName().ToString();
	}
	else if (OverwritableAction == 2)
	{
		Action = FName("SpellSlot0");
		SpellSlot0AssignedKey = Key.GetFName().ToString();
	}
	else
	{
		Action = SDConstants::NO_SPELL;
	}

	if (Action.Compare(FName(SDConstants::NO_SPELL)) != 0)
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

FKey ASDNetPlayerControllerProxy::GetKeyForAction(FName ActionName)
{
	TArray <FInputActionKeyMapping> AllMappedKeys = PlayerInput->GetKeysForAction(ActionName);
	if (AllMappedKeys.Num() > 0)
	{
		return AllMappedKeys[0].Key;
	}
	else
	{
		return FKey(TEXT("Escape"));
	}
}

void ASDNetPlayerControllerProxy::OnClosePlayerMenu()
{
	SetHotkeyMenuCanBeOpened(true);
}

void ASDNetPlayerControllerProxy::OnCloseMpMenu()
{

}

void ASDNetPlayerControllerProxy::UseSpell(FName SpellName)
{
	if (SDConstants::SpellNames.Contains(SpellName))
	{

	}

	if (SpellName.IsEqual(SDConstants::CHEAT_SPELL))
	{

	}
}

void ASDNetPlayerControllerProxy::OnMovementKeyPressed()
{
	bMoveToLocation = true;
}

void ASDNetPlayerControllerProxy::OnMovementKeyReleased()
{
	bMoveToLocation = false;
}

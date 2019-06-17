// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDGameInstance.h"
#include "SDGameState.h"
#include "SDFireBoltSpell.h"
#include "SDCelestialFragmentSpell.h"
#include "SDSunBurstSpell.h"
#include "SDSlash.h"
#include "SDRangedAttack.h"
#include "SDNetPlayerControllerProxy.h"


void ASDNetPlayerControllerProxy::PreClientTravel(const FString & PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	isTravelling = true;
	const AActor* SCOwner = ServerCharacter->GetNetOwner();
	ServerCharacter->ServerDestroy();
}

bool ASDNetPlayerControllerProxy::CastSpell_Validate(ASDBaseSpell *SpellTocast, FHitResult Hit)
{
	return true;
}

void ASDNetPlayerControllerProxy::CastSpell_Implementation(ASDBaseSpell *SpellToCast, FHitResult Hit)
{
	FVector TargetLocation(0.0f, 0.0f, 0.0f);
	if (SpellToCast != nullptr)
	{	
		if (Hit.bBlockingHit)
		{
			TargetLocation = Hit.Location;
		}
		if (ServerController == nullptr)
		{
			ServerController = PlayerProxy->GetServerController();
		}
		if (ServerController != nullptr)
		{
			SpellToCast->Init(dynamic_cast<APawn *> (GetPawn()));
			SpellToCast->SetTeamId(1);
			SpellToCast->CastSpell(TargetLocation);
		}
	}
}

ASDNetPlayerControllerProxy::ASDNetPlayerControllerProxy()
{
	bMoveToLocation = false;
	bReplicates = true;
	bShowMouseCursor = true;
	HotkeyMenuCanBeOpened = true;
	InventoryMenuCanBeOpened = true;
	MpMenuCanBeOpened = true;
	OverwritableAction = SDConstants::HotKeyOverrides::NO_ACTION_WRITABLE;
	StandardInput.SetHideCursorDuringCapture(false);
	isTravelling = false;
}

void ASDNetPlayerControllerProxy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASDNetPlayerControllerProxy, ServerController);
	DOREPLIFETIME(ASDNetPlayerControllerProxy, ServerCharacter);
	DOREPLIFETIME(ASDNetPlayerControllerProxy, PlayerProxy);

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
			if (Hit.Actor.IsValid())
			{
				if (Hit.Actor->Implements<ISDClickable::UClassType>())
				{
					ISDClickable::Execute_ReceiveClick(Hit.Actor.Get(), this);
				}
			}

			MoveToLocation(Hit.Location);
		}
	}
	if (NextCommand != nullptr)
	{
		CurrentCommand = NextCommand;
		NextCommand = nullptr;
		(this->*CurrentCommand)(NextCommandVector);
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

	InputComponent->BindAction("SpellSlot1", IE_Pressed, this, &ASDNetPlayerControllerProxy::OnSpellSlot1Pressed);
	InputComponent->BindAction("SpellSlot1", IE_Released, this, &ASDNetPlayerControllerProxy::OnSpellSlot1Released);

	InputComponent->BindAction("SpellSlot2", IE_Pressed, this, &ASDNetPlayerControllerProxy::OnSpellSlot2Pressed);
	InputComponent->BindAction("SpellSlot2", IE_Released, this, &ASDNetPlayerControllerProxy::OnSpellSlot2Released);

	InputComponent->BindAction("SpellSlot3", IE_Pressed, this, &ASDNetPlayerControllerProxy::OnSpellSlot3Pressed);
	InputComponent->BindAction("SpellSlot3", IE_Released, this, &ASDNetPlayerControllerProxy::OnSpellSlot3Released);

	InputComponent->BindAction("LaunchMpMenu", IE_Pressed, this, &ASDNetPlayerControllerProxy::OnLaunchMpMenu);
	InputComponent->BindAction("LaunchMpMenu", IE_Released, this, &ASDNetPlayerControllerProxy::OnCloseMpMenu);

	InputComponent->BindAction("LaunchInventoryMenu", IE_Pressed, this, &ASDNetPlayerControllerProxy::OnLaunchInventoryMenu);
	InputComponent->BindAction("LaunchInventoryMenu", IE_Released, this, &ASDNetPlayerControllerProxy::OnCloseInventoryMenu);

	InputComponent->BindAction("SwapWeapons", IE_Pressed, this, &ASDNetPlayerControllerProxy::SwapWeapons);

}

void ASDNetPlayerControllerProxy::BeginPlay()
{
	USDGameInstance *GameInstance = dynamic_cast<USDGameInstance *>(GetGameInstance());
	TArray<FQuestStruct> QuestStructs;

	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("authority on proxy controller beginplay"));
	}

	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer)
	{
		if (LocalPlayer->GetControllerId() != 0)
		{
			GameInstance->ServerCriticalSection.Lock();
			LocalPlayer->SetControllerId(GameInstance->GetPlayerIDOnJoin());
			GameInstance->ServerCriticalSection.Unlock();
		}
	}

	PlayerProxy = (ASDNetPlayerProxy *)GetPawn();
	if (PlayerProxy != nullptr)
	{
		if (LocalPlayer)
		{
			PlayerProxy->SetPlayerId(LocalPlayer->GetControllerId());
		}
		UE_LOG(LogTemp, Warning, TEXT("successfully grabbed player proxy instance!"));
		ServerController = PlayerProxy->GetServerController();
		if (ServerController != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("successfully grabbed player server controller!"));
		}
	}
	
	if (ServerController == nullptr && PlayerProxy != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Getting ServerController from Pawn Proxy"));
		ServerController = PlayerProxy->GetServerController();
	}

	if (GameInstance != nullptr)
	{
		GameInstance->OnItemPickup.AddDynamic(this, &ASDNetPlayerControllerProxy::OnItemPickup);
		GameInstance->OnItemEquipped.AddDynamic(this, &ASDNetPlayerControllerProxy::OnItemEquipped);
	}
	
	if (IsLocalPlayerController())
	{
		if (wMpMenu)
		{
			MpMenu = CreateWidget<UUserWidget>(this, wMpMenu);
		}

		if (wPlayerGameMenu)
		{
			PlayerGameMenu = CreateWidget<UUserWidget>(this, wPlayerGameMenu);
		}

		if (wInventoryMenu)
		{
			InventoryMenu = CreateWidget<UUserWidget>(this, wInventoryMenu);
		}

		if (wPlayerHud)
		{
			PlayerHud = CreateWidget<UUserWidget>(this, wPlayerHud);
			if (PlayerHud)
			{
				PlayerHud->AddToViewport();
			}
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
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	CastSpell(PlayerProxy->GetSpellSlot(0), Hit);
}

void ASDNetPlayerControllerProxy::OnSpellSlot0Released()
{

}

void ASDNetPlayerControllerProxy::OnSpellSlot1Pressed()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	CastSpell(PlayerProxy->GetSpellSlot(1), Hit);
	InteractionTarget = nullptr;
}

void ASDNetPlayerControllerProxy::OnSpellSlot1Released()
{

}

void ASDNetPlayerControllerProxy::OnSpellSlot2Pressed()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	CastSpell(PlayerProxy->GetSpellSlot(2), Hit);
	InteractionTarget = nullptr;
}

void ASDNetPlayerControllerProxy::OnSpellSlot2Released()
{

}

void ASDNetPlayerControllerProxy::OnSpellSlot3Pressed()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	CastSpell(PlayerProxy->GetSpellSlot(3), Hit);
	InteractionTarget = nullptr;
}

void ASDNetPlayerControllerProxy::OnSpellSlot3Released()
{

}

void ASDNetPlayerControllerProxy::MoveToLocation_Implementation(FVector target)
{
	if (!ServerController->MoveToLocation(target))
	{
		NextCommand = &ASDNetPlayerControllerProxy::MoveToLocation;
		NextCommandVector = target;
	}
}

bool ASDNetPlayerControllerProxy::MoveToLocation_Validate(FVector target)
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
			GetPawn()->DisableInput(this);
			PlayerGameMenu->AddToViewport();
		}
		else
		{
			PlayerGameMenu->RemoveFromViewport();
		}
	}
}

void ASDNetPlayerControllerProxy::OnLaunchMpMenu()
{
	if (MpMenu != nullptr)
	{
		if (!MpMenu->IsInViewport() && MpMenuCanBeOpened)
		{
			UE_LOG(LogTemp, Warning, TEXT("addding MpMenu to viewport"));
			SetMpMenuCanBeOpened(false);
			MpMenu->AddToViewport();
			//SetInputMode(KeyRebindInput);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("removing MpMenu from viewport"));
			MpMenu->RemoveFromViewport();
//			SetInputMode(StandardInput);
		}
	}
}

void ASDNetPlayerControllerProxy::OnLaunchInventoryMenu()
{
	if (InventoryMenu != nullptr)
	{
		if (!InventoryMenu->IsInViewport())
		{
			InventoryMenu->AddToViewport();
		}
		else
		{
			if (!InventoryMenu->IsVisible() && InventoryMenuCanBeOpened)
			{
				SetInventoryMenuCanBeOpened(false);
				InventoryMenu->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				InventoryMenu->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}

}

void ASDNetPlayerControllerProxy::SetHotkeyMenuCanBeOpened(bool NewValue)
{
	HotkeyMenuCanBeOpened = NewValue;
}

void ASDNetPlayerControllerProxy::SetInventoryMenuCanBeOpened(bool NewValue)
{
	InventoryMenuCanBeOpened = NewValue;
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

void ASDNetPlayerControllerProxy::OnItemPickup(ASDBaseEquipment* PickedUp)
{
	USDGameInstance* GameInstance = dynamic_cast<USDGameInstance *>(GetGameInstance());
	if (GameInstance != nullptr)
	{
		GameInstance->AddItemToPlayerInventory(0, PickedUp);
	}
	this->AddEquipmentToMenu(PickedUp);
}

void ASDNetPlayerControllerProxy::OnItemEquipped(ASDBaseEquipment* Equipped, bool MainHand)
{
	USDInventoryWidget *CastWidget = dynamic_cast<USDInventoryWidget *>(InventoryMenu);
	if (CastWidget != nullptr)
	{
		CastWidget->AddItemAsEquipped(Equipped, MainHand);
	}
}

void ASDNetPlayerControllerProxy::OnServerCharLoaded()
{
	USDGameInstance* GameInstance = dynamic_cast<USDGameInstance *>(GetGameInstance());
	TArray<FQuestStruct> QuestStructs = GameInstance->GetPlayerQuests(ServerCharacter->GetPlayerID()).QuestArray;
	for (FQuestStruct Q : QuestStructs)
	{
		ActiveQuests.Add(ConvertQuestStruct(Q));
	}
}

void ASDNetPlayerControllerProxy::SetControllerInputModeGame()
{
	SetInputMode(StandardInput);
}

void ASDNetPlayerControllerProxy::ToggleLoadingScreen()
{
	if (LoadingScreen != nullptr)
	{
		if (!LoadingScreen->IsInViewport())
		{
			LoadingScreen->AddToViewport();
		}
		else
		{
			LoadingScreen->RemoveFromParent();
		}
	}
}

void ASDNetPlayerControllerProxy::HandleLevelLoaded()
{
	UE_LOG(LogTemp, Warning, TEXT("Level loaded for controller!"));
	AActor* SpawnAt = nullptr;
	if (GetWorld() && isTravelling)
	{
		isTravelling = false;
		if (GetWorld()->GetAuthGameMode())
		{
			SpawnAt = GetWorld()->GetAuthGameMode()->ChoosePlayerStart(this);
			APawn *MyPawn = GetPawn();
			if (MyPawn != nullptr)
			{
				MyPawn->SetActorLocation(SpawnAt->GetActorLocation());
				SpawnServerCharacter();

				OnServerCharLoaded();
			}
		}
	}
}

FTimerHandle ASDNetPlayerControllerProxy::GetSpellTimer(uint8 SpellSlot)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Ayyy"));
	}

	if (SpellSlot > 3 || SpellSlot < 0)
	{
		if (PlayerProxy == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerProxy null in GetSpellTimer"));
		}
		else if (PlayerProxy->GetSpellSlot(0) == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("SpellSlot0 null in GetSpellTimer"));
		}
		return PlayerProxy->GetSpellSlot(0)->TimerHandler;
	}
	if (PlayerProxy == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerProxy null in GetSpellTimer"));
	}
	else if (PlayerProxy->GetSpellSlot(SpellSlot) == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpellSlot%d null in GetSpellTimer"), &SpellSlot);
	}
	return PlayerProxy->GetSpellSlot(SpellSlot)->TimerHandler;
}

TArray<USDBaseQuest *> ASDNetPlayerControllerProxy::GetAllQuests()
{
	return ActiveQuests;
}

void ASDNetPlayerControllerProxy::AddQuest(USDBaseQuest * Quest)
{
	ActiveQuests.Add(Quest);
	USDGameInstance* GameInstance = dynamic_cast<USDGameInstance *>(GetGameInstance());
	if (GameInstance != nullptr)
	{
		GameInstance->AddQuestToPlayer(ServerCharacter->GetPlayerID(), Quest);
	}
}

void ASDNetPlayerControllerProxy::TravelToLevel(FName LevelToLoad)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempting to do ServerTravel"));
	if (HasAuthority())
	{
		bool result = GetWorld()->ServerTravel(LevelToLoad.ToString());
		if (result)
		{
			UE_LOG(LogTemp, Warning, TEXT("returned true"));
		}
		else
		{
			UGameplayStatics::OpenLevel(GetWorld(), LevelToLoad);
			UE_LOG(LogTemp, Warning, TEXT("returned false"));
		}
	}
}

AActor* ASDNetPlayerControllerProxy::GetInteractionTarget()
{
	return InteractionTarget;
}

void ASDNetPlayerControllerProxy::SetInteractionTarget(AActor* Target)
{
	InteractionTarget = Target;
	ServerCharacter->SetInteractionTarget(Target);
}

void ASDNetPlayerControllerProxy::GetSeamlessTravelActorList(bool bToEntry, TArray <class AActor *> &ActorList)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("ASDNetPlayerControllerProxy::GetSeamlessTravelActorList(ToEntry: First%s)"), (bToEntry ? TEXT("True") : TEXT("False"))));
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("ASDNetPlayerControllerProxy::GetSeamlessTravelActorList(ToEntry: Second%s)"), (bToEntry ? TEXT("True") : TEXT("False"))));

	USDGameInstance *GameInstance = dynamic_cast<USDGameInstance *>(GetGameInstance());
	ASDNetPlayerProxy* ProxyPawn = dynamic_cast<ASDNetPlayerProxy *>(GetPawn());
	
	if (ProxyPawn != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("ASDNetPlayerControllerProxy::GetSeamlessTravelActorList(ToEntry: %s): ProxyPawn is not null"), (bToEntry ? TEXT("True") : TEXT("False"))));
		ServerCharacter = ProxyPawn->GetServerCharacter();
	}
	if (bToEntry)
	{
		// Here we place things on ActorList
		if (ServerCharacter != nullptr)
		{
			for (ASDBaseEquipment* Equipment : ServerCharacter->CarriedItems)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("ASDNetPlayerControllerProxy::GetSeamlessTravelActorList(ToEntry: %s): Placing item in ActorList"), (bToEntry ? TEXT("True") : TEXT("False"))));
				ActorList.Add(Equipment);
			}
		}
	}
	else
	{
		ASDBaseEquipment* Equipment;
		// Here we use ActorList to repopulate our controller's data

		if (GameInstance != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("ASDNetPlayerControllerProxy::GetSeamlessTravelActorList(ToEntry: %s): ActorList contains %d items"),
				(bToEntry ? TEXT("True") : TEXT("False")), ActorList.Num()));

			for (AActor* A : ActorList)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("ASDNetPlayerControllerProxy::GetSeamlessTravelActorList(ToEntry: %s): ActorList loop"), (bToEntry ? TEXT("True") : TEXT("False"))));
				if (A != nullptr)
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("ASDNetPlayerControllerProxy::GetSeamlessTravelActorList(ToEntry: %s): Actor name is %s"), 
						(bToEntry ? TEXT("True") : TEXT("False")), *A->GetName()));
				}
				Equipment = dynamic_cast<ASDBaseEquipment *>(A);
				if (Equipment != nullptr)
				{
					GameInstance->AddItemToPlayerInventory(0, Equipment);
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("ASDNetPlayerControllerProxy::GetSeamlessTravelActorList(ToEntry: %s): Item added to GameInstance PlayerInventory"), (bToEntry ? TEXT("True") : TEXT("False"))));
				}
			}
		}
	}
}

void ASDNetPlayerControllerProxy::Possess(APawn * InPawn)
{
	APlayerController::Possess(InPawn);
}

void ASDNetPlayerControllerProxy::OnClosePlayerMenu()
{
	SetHotkeyMenuCanBeOpened(true);
}

void ASDNetPlayerControllerProxy::OnCloseMpMenu()
{
	MpMenuCanBeOpened = true;
}

void ASDNetPlayerControllerProxy::OnCloseInventoryMenu()
{
	InventoryMenuCanBeOpened = true;
}

void ASDNetPlayerControllerProxy::AddEquipmentToMenu(ASDBaseEquipment *HeldEquipment)
{
	USDInventoryWidget *CastWidget = dynamic_cast<USDInventoryWidget *>(InventoryMenu);
	if (CastWidget != nullptr)
	{
		CastWidget->AddButtonToPanel(HeldEquipment);
		//CastWidget->SetEquipment(HeldEquipment);
	}
}

void ASDNetPlayerControllerProxy::OnMovementKeyPressed()
{
	bMoveToLocation = true;
	InteractionTarget = nullptr;
}

void ASDNetPlayerControllerProxy::OnMovementKeyReleased()
{
	bMoveToLocation = false;
}

void ASDNetPlayerControllerProxy::SwapWeapons()
{
	if (ServerCharacter == nullptr)
	{
		ServerCharacter = PlayerProxy->GetServerCharacter();
	}
	ServerCharacter->SwapWeapons();
}

bool ASDNetPlayerControllerProxy::SpawnServerCharacter()
{
	USDGameInstance *GameInstance = dynamic_cast<USDGameInstance *>(GetGameInstance());
	if (ServerCharacter != nullptr) return false;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = GetPawn();
	FVector SpawnLocation = GetPawn()->GetActorLocation();

	ServerCharacter = GetWorld()->SpawnActor<ASDNetPlayerPawn>(NetCharacterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer)
	{
		ServerCharacter->SetPlayerID(LocalPlayer->GetControllerId());
	}
	if (ServerCharacter != nullptr)
	{
		ServerController = GetWorld()->SpawnActor<ASDNetPlayerController>(NetControllerClass, SpawnParams);
		ServerController->Possess(ServerCharacter);
		UPlayer* ServerPlayer = ServerController->GetNetOwningPlayer();
		ASDNetPlayerProxy* Proxy = dynamic_cast<ASDNetPlayerProxy *>(GetPawn());
		if (Proxy != nullptr && ServerController != nullptr)
		{
			Proxy->SetServerController(ServerController);
		}
		if (ServerController != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Successfully created server controller!"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not create server controller!"));
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool ASDNetPlayerControllerProxy::getIsTravelling()
{
	return isTravelling;
}

USDBaseQuest* ASDNetPlayerControllerProxy::ConvertQuestStruct(FQuestStruct QuestStruct)
{
	USDBaseQuest* ReturnQuest = NewObject<USDBaseQuest>();
	ReturnQuest->SetRequiredItemName(QuestStruct.RequiredItemName);
	ReturnQuest->QuestStatus = QuestStruct.Status;
	ReturnQuest->QuestName = QuestStruct.QuestName;

	return ReturnQuest;
}

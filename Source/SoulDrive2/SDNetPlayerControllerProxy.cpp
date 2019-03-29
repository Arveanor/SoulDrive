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
	//SetInputMode(KeyRebindInput);
}

void ASDNetPlayerControllerProxy::CastSpell(ASDBaseSpell *SpellToCast)
{
	FVector TargetLocation(0.0f, 0.0f, 0.0f);
	if (SpellToCast != nullptr)
	{
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);
		
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
	bShowMouseCursor = true;
	HotkeyMenuCanBeOpened = true;
	InventoryMenuCanBeOpened = true;
	MpMenuCanBeOpened = true;
	OverwritableAction = SDConstants::HotKeyOverrides::NO_ACTION_WRITABLE;
	StandardInput.SetHideCursorDuringCapture(false);

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
	
	if (ServerController == nullptr && PlayerProxy != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Getting ServerController from Pawn Proxy"));
		ServerController = PlayerProxy->GetServerController();
	}

	if (GameInstance != nullptr)
	{
		GameInstance->OnItemPickup.AddDynamic(this, &ASDNetPlayerControllerProxy::OnItemPickup);
		GameInstance->OnItemEquipped.AddDynamic(this, &ASDNetPlayerControllerProxy::OnItemEquipped);
		GameInstance->OnServerCharLoaded.AddDynamic(this, &ASDNetPlayerControllerProxy::OnServerCharLoaded);
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

// 		if (wLoadingScreen)
// 		{
// 			LoadingScreen = CreateWidget<UUserWidget>(this, wLoadingScreen);
// 			if (LoadingScreen)
// 			{
// 				LoadingScreen->AddToViewport();
// 			}
// 		}
	}

	if (SDConstants::CheatMode)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Instigator = GetPawn();
		SpellSlot0 = GetWorld()->SpawnActor<ASDCelestialFragmentSpell>(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnInfo);
		SpellSlot1 = GetWorld()->SpawnActor<ASDFireBoltSpell>(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnInfo);
		SpellSlot2 = GetWorld()->SpawnActor<ASDSlash>(FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f), SpawnInfo);
		SpellSlot3 = GetWorld()->SpawnActor<ASDRangedAttack>(FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f), SpawnInfo);
		ASDCheatSpell* ChildRef = dynamic_cast<ASDCheatSpell*>(SpellSlot0);

// 		if (ChildRef != nullptr && ServerController != nullptr)
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("Initializing SpellSlot0"));
// 			ChildRef->Init(ServerController);
// 		}
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
	FVector TargetLocation(0.0f, 0.0f, 0.0f);
	if (SpellSlot0 != nullptr)
	{
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);
		if (Hit.bBlockingHit)
		{
			TargetLocation = Hit.Location;
		}
		if (ServerController == nullptr)
		{
			ServerController = PlayerProxy->GetServerController();
		}

		SpellSlot0->Init(dynamic_cast<APawn *> (GetPawn()));
		SpellSlot0->SetTeamId(1);
		SpellSlot0->CastSpell(TargetLocation);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SpellSlot0 not being cast"));
	}
}

void ASDNetPlayerControllerProxy::OnSpellSlot0Released()
{

}

void ASDNetPlayerControllerProxy::OnSpellSlot1Pressed()
{
	CastSpell(SpellSlot1);
}

void ASDNetPlayerControllerProxy::MoveToLocation_Implementation(FVector target)
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
	if (!ServerController->MoveToLocation(target))
	{
		NextCommand = &ASDNetPlayerControllerProxy::MoveToLocation;
		NextCommandVector = target;
	}
}

void ASDNetPlayerControllerProxy::OnSpellSlot1Released()
{

}

void ASDNetPlayerControllerProxy::OnSpellSlot2Pressed()
{
	CastSpell(SpellSlot2);
}

void ASDNetPlayerControllerProxy::OnSpellSlot2Released()
{

}

void ASDNetPlayerControllerProxy::OnSpellSlot3Pressed()
{
	CastSpell(SpellSlot3);
}

void ASDNetPlayerControllerProxy::OnSpellSlot3Released()
{

}

bool ASDNetPlayerControllerProxy::MoveToLocation_Validate(FVector target)
{
	return true;
}

void ASDNetPlayerControllerProxy::OnLaunchPlayerMenu()
{
	if (HasAuthority())
	{
//		return;
	}
	if (PlayerGameMenu != nullptr)
	{
		if (!PlayerGameMenu->IsInViewport() && HotkeyMenuCanBeOpened)
		{
			SetHotkeyMenuCanBeOpened(false);
			GetPawn()->DisableInput(this);
			PlayerGameMenu->AddToViewport();
			//PlayerGameMenu->SetKeyboardFocus();
			//KeyRebindInput = KeyRebindInput.SetWidgetToFocus(PlayerGameMenu->TakeWidget());	
			//SetInputMode(KeyRebindInput);
		}
		else
		{
			PlayerGameMenu->RemoveFromViewport();
//			SetInputMode(StandardInput);
		}
	}
}

void ASDNetPlayerControllerProxy::OnLaunchMpMenu()
{
	if (HasAuthority())
	{
//		return;
	}
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

void ASDNetPlayerControllerProxy::OnServerCharLoaded(uint8 PlayerId)
{
	if (PlayerProxy == nullptr) return;
	ServerCharacter = PlayerProxy->GetServerCharacter();
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
// 	UE_LOG(LogTemp, Warning, TEXT("Level loaded for controller!"));
// 	AActor* SpawnAt = nullptr;
// 	if(GetWorld())
// 		if(GetWorld()->GetAuthGameMode())
// 			SpawnAt = GetWorld()->GetAuthGameMode()->ChoosePlayerStart(this);
// 	APawn *MyPawn = GetPawn();
// 	if (MyPawn != nullptr)
// 	{
// 		MyPawn->SetActorLocation(SpawnAt->GetActorLocation());
// 	}
// 	
// 	ASDNetPlayerProxy* SDNetPawn = dynamic_cast<ASDNetPlayerProxy *>(MyPawn);
// 	if (SDNetPawn != nullptr)
// 	{
// 		SDNetPawn->SpawnServerCharacter();
// 	}
//	ServerCharacter = GetWorld()->SpawnActor<ASDNetPlayerProxy>(NetCharacterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

/*	this->ToggleLoadingScreen();*/

}

FTimerHandle ASDNetPlayerControllerProxy::GetSpellTimer(uint8 SpellSlot)
{
	switch(SpellSlot)
	{
		case 0:
			return SpellSlot0->TimerHandler;
			break;
		case 1:
			return SpellSlot1->TimerHandler;
			break;
		case 2:
			return SpellSlot2->TimerHandler;
			break;
		default:
			//UE_LOG(LogTemp, ERROR, TEXT("Unable to retrieve spell timer from spellslot %d"), SpellSlot);
			return SpellSlot0->TimerHandler;
	}
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

void ASDNetPlayerControllerProxy::GetSeamlessTravelActorList(bool bToEntry, TArray <class AActor *> &ActorList)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("ASDNetPlayerControllerProxy::GetSeamlessTravelActorList(ToEntry: First%s)"), (bToEntry ? TEXT("True") : TEXT("False"))));
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("ASDNetPlayerControllerProxy::GetSeamlessTravelActorList(ToEntry: Second%s)"), (bToEntry ? TEXT("True") : TEXT("False"))));

	USDGameInstance *GameInstance = dynamic_cast<USDGameInstance *>(GetGameInstance());
	ASDNetPlayerPawn* ServerPawn = nullptr;
	ASDNetPlayerProxy* ProxyPawn = dynamic_cast<ASDNetPlayerProxy *>(GetPawn());
	
	if (ProxyPawn != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("ASDNetPlayerControllerProxy::GetSeamlessTravelActorList(ToEntry: %s): ProxyPawn is not null"), (bToEntry ? TEXT("True") : TEXT("False"))));
		ServerPawn = ProxyPawn->GetServerCharacter();
	}
	if (bToEntry)
	{
		// Here we place things on ActorList
		if (ServerPawn != nullptr)
		{
			for (ASDBaseEquipment* Equipment : ServerPawn->CarriedItems)
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

USDBaseQuest* ASDNetPlayerControllerProxy::ConvertQuestStruct(FQuestStruct QuestStruct)
{
	USDBaseQuest* ReturnQuest = NewObject<USDBaseQuest>();
	ReturnQuest->SetRequiredItemName(QuestStruct.RequiredItemName);
	ReturnQuest->QuestStatus = QuestStruct.Status;
	ReturnQuest->QuestName = QuestStruct.QuestName;

	return ReturnQuest;
}

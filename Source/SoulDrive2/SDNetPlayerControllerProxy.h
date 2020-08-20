// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "Networking.h"
#include "SDBaseQuest.h"
#include "SDNetPlayerControllerProxy.generated.h"

class UUserWidget;
class ASDNetPlayerProxy;
class USDInventoryWidget;
class ASDBaseSpell;
class ASDBaseEquipment;
class ASDBaseWeapon;
class ASDNetPlayerController;

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDNetPlayerControllerProxy : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	void OnDebugActionPressed();
	void OnDebugActionReleased();
	void OnSpellSlot0Pressed();
	void OnSpellSlot0Released();
	void OnSpellSlot1Pressed();
	void OnSpellSlot1Released();
	void OnSpellSlot2Pressed();
	void OnSpellSlot2Released();
	void OnSpellSlot3Pressed();
	void OnSpellSlot3Released();

	/* Stores actions to be executed when the player has control again */
	TQueue<void (ASDNetPlayerControllerProxy::*)(FVector)> InputQueue;

	void (ASDNetPlayerControllerProxy::*NextCommand)(FVector);
	void (ASDNetPlayerControllerProxy::*CurrentCommand)(FVector);
	FVector NextCommandVector;

	UPROPERTY(BlueprintReadWrite, Category = "Widgets")
	UUserWidget* PlayerGameMenu;

	UPROPERTY(BlueprintReadWrite, Category = "Widgets")
	UUserWidget* MpMenu;

	UPROPERTY(BlueprintReadWrite, Category = "Widgets")
	UUserWidget* InventoryMenu;

	UPROPERTY(BlueprintReadWrite, Category = "Widgets")
	UUserWidget* PlayerHud;

	UPROPERTY(BlueprintReadWrite, Category = "Widgets")
	UUserWidget* LoadingScreen;

	UPROPERTY(BlueprintReadWrite, Category = "Testing")
	int discoveringTransitionLevels;

	UFUNCTION(Category = "Networking")
	void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void CastSpell(ASDBaseSpell *SpellToCast, FHitResult Hit);	

public:
	ASDNetPlayerControllerProxy();

	UFUNCTION(Server, Reliable, WithValidation)
	void MoveToLocation(FVector target);
	void MoveToLocation_Implementation(FVector target);
	bool MoveToLocation_Validate(FVector target);

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void OnLaunchPlayerMenu();

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void OnLaunchMpMenu();

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void OnLaunchInventoryMenu();

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void SetHotkeyMenuCanBeOpened(bool NewValue);

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void SetInventoryMenuCanBeOpened(bool NewValue);

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void SetMpMenuCanBeOpened(bool NewValue);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OverrideHotkey(const FKey Key);

	UFUNCTION(BlueprintCallable, Category = "Input")
	FKey GetKeyForAction(FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OnItemPickup(ASDBaseEquipment *PickedUp);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OnItemEquipped(ASDBaseEquipment* Equipped, bool MainHand);

	UFUNCTION(BlueprintCallable, Category = "Player")
	void OnServerCharLoaded();

	UFUNCTION(BlueprintCallable, Category = "Levels")
	void SetControllerInputModeGame();

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void ToggleLoadingScreen();

	UFUNCTION(BlueprintCallable, Category = "Levels")
	void HandleLevelLoaded();

	UFUNCTION(BlueprintCallable, Category = "Spells")
	FTimerHandle GetSpellTimer(uint8 SpellSlot);

	UFUNCTION(BlueprintCallable, Category = "Quests")
	TArray<USDBaseQuest *> GetAllQuests();
	
	UFUNCTION(BlueprintCallable, Category = "Quests")
	void AddQuest(USDBaseQuest* Quest);

	UFUNCTION(BlueprintCallable, Category = "Levels")
	void TravelToLevel(FName LevelToLoad);

	UFUNCTION(BlueprintCallable)
	AActor* GetInteractionTarget();
	void SetInteractionTarget(AActor* Target);

	UFUNCTION(BlueprintCallable)
	void LaunchTCPServer();

	void PawnLeavingGame() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wPlayerGameMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wMpMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wInventoryMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wPlayerHud;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wLoadingScreen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	uint8 OverwritableAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TSubclassOf<ASDNetPlayerPawn> NetCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TSubclassOf<ASDNetPlayerController> NetControllerClass;

	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray <class AActor *> &ActorList) override;
	virtual void Possess(APawn* InPawn) override;

	void OnClosePlayerMenu();
	void OnCloseMpMenu();
	void OnCloseInventoryMenu();
	bool SpawnServerCharacter();
	void CreateReconnectSave();
	bool getIsTravelling();

	void AddEquipmentToMenu(ASDBaseEquipment *HeldEquipment);
private:

	UFUNCTION()
	void TCPSocketListener();

	UFUNCTION()
	void AlertServer();
	
	void TCPConnectionListener();
	void OnMovementKeyPressed();
	void OnMovementKeyReleased();

	FString StringFromBinaryArray(TArray<uint8> BinaryArray);

	void SwapWeapons();
	USDBaseQuest* ConvertQuestStruct(FQuestStruct QuestStruct);

	// Server side character and controller to handle real logic for this proxy
	UPROPERTY(replicated)
	ASDNetPlayerController *ServerController;
	UPROPERTY(replicated)
	ASDNetPlayerPawn *ServerCharacter;
	UPROPERTY(replicated)
	ASDNetPlayerProxy *PlayerProxy;

	FSocket* ListenerSocket;
	FSocket* ConnectionSocket;
	FIPv4Endpoint RemoteAddressForConnection;
	FTimerHandle TCPConnectionTimer;
	FTimerHandle TCPSocketTimer;
	FString DebugActionAssignedKey;
	FString SpellSlot0AssignedKey;
	bool HotkeyMenuCanBeOpened;
	bool MpMenuCanBeOpened;
	bool InventoryMenuCanBeOpened;
	bool bMoveToLocation;
	bool isTravelling; // going to a new level?

	FInputModeUIOnly KeyRebindInput;
	FInputModeGameAndUI StandardInput;

	UPROPERTY()
	TArray<USDBaseQuest* > ActiveQuests;	

	AActor* InteractionTarget;
};

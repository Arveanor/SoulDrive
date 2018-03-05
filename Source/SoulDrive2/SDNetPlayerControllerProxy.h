// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "SDNetPlayerController.h"
#include "Blueprint/UserWidget.h"
//#include "SDNetPlayerPawn.h"
#include "SDNetPlayerProxy.h"
#include "SDBaseEquipment.h"
#include "SDConstants.h"
#include "SDBaseSpell.h"
#include "SDCheatSpell.h"
#include "SDInventoryWidget.h"
#include "SDNetPlayerControllerProxy.generated.h"

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

	UPROPERTY(BlueprintReadWrite, Category = "Widgets")
	UUserWidget* PlayerGameMenu;

	UPROPERTY(BlueprintReadWrite, Category = "Widgets")
	UUserWidget* MpMenu;

	UPROPERTY(BlueprintReadWrite, Category = "Widgets")
	UUserWidget* InventoryMenu;

	UPROPERTY(BlueprintReadWrite, Category = "Widgets")
	UUserWidget* PlayerHud;
	
public:
	ASDNetPlayerControllerProxy();

	UFUNCTION(Server, Reliable, WithValidation)
	void MoveToLocation(FHitResult HitResult);
	void MoveToLocation_Implementation(FHitResult HitResult);
	bool MoveToLocation_Validate(FHitResult HitResult);

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

	void PickupItem(const ASDBaseEquipment &PickedUpItem);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wPlayerGameMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wMpMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wInventoryMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wPlayerHud;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	uint8 OverwritableAction;

	void OnClosePlayerMenu();
	void OnCloseMpMenu();
	void OnCloseInventoryMenu();
	
	void AddEquipmentToMenu(ASDBaseEquipment *HeldEquipment);
	void UseSpell(FName SpellName);
private:

	void OnMovementKeyPressed();
	void OnMovementKeyReleased();

	// Server side character and controller to handle real logic for this proxy
	ASDNetPlayerController *ServerController;
	ASDNetPlayerPawn *ServerCharacter;
	ASDNetPlayerProxy *PlayerProxy;

	FString DebugActionAssignedKey;
	FString SpellSlot0AssignedKey;
	bool HotkeyMenuCanBeOpened;
	bool MpMenuCanBeOpened;
	bool InventoryMenuCanBeOpened;
	bool bMoveToLocation;

	FInputModeUIOnly KeyRebindInput;
	FInputModeGameAndUI StandardInput;

	ASDBaseSpell* SpellSlot0;
	ASDBaseSpell* SpellSlot1;
	ASDBaseSpell* SpellSlot2;
	ASDBaseSpell* SpellSlot3;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "SDConstants.h"
#include "SDPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	bool bMoveToCursor;

	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
	void OnDebugActionPressed();
	void OnDebugActionReleased();
	void MoveToCursor();

public:

	ASDPlayerController();
	void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OverrideHotkey(const FKey Key);

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void OnLaunchPlayerMenu();

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	void SetMenuCanBeOpened(bool NewValue);

	void OnClosePlayerMenu();

	UFUNCTION(Reliable, Server, WithValidation)
	void RequestMoveToLocation(AController *TargetController, const FVector &Goal);
	void RequestMoveToLocation_Implementation(AController *TargetController, const FVector &Goal);
	bool RequestMoveToLocation_Validate(AController *TargetController, const FVector &Goal);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	uint8 OverwritableAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wPlayerGameMenu;

private:

	FString DebugActionAssignedKey;
	bool MenuCanBeOpened;
	FInputModeUIOnly KeyRebindInput;
	FInputModeGameAndUI StandardInput;
	UUserWidget* PlayerGameMenu;
};
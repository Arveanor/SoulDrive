// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Class.h"
#include "SDNetPlayerPawn.h"
#include "SDNetPlayerController.h"
#include "SDBaseEquipment.h"
#include "SDConstants.h"
#include "SDNetPlayerProxy.generated.h"

class ASDBaseSpell;
class ASDNetPlayerControllerProxy;

UCLASS()
class SOULDRIVE2_API ASDNetPlayerProxy : public ASDBasePawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASDNetPlayerProxy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UFUNCTION(Server, Unreliable, WithValidation)
	void SetPortalTarget(const FString& InURL, ASDPortal* Portal);

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* PlayerCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* MainCameraBoom;

// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Player, meta = (AllowPrivateAccess = "true"))
// 	class USceneComponent *SceneRoot;

	UPROPERTY(replicated)
	ASDNetPlayerPawn *ServerCharacter;

	UPROPERTY(replicated)
	ASDNetPlayerController *ServerController;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), replicated)
	ASDBaseSpell* SpellSlot0;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), replicated)
	ASDBaseSpell* SpellSlot1;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), replicated)
	ASDBaseSpell* SpellSlot2;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), replicated)
	ASDBaseSpell* SpellSlot3;

	UPROPERTY()
	FVector LerpTarget;

	uint8 PlayerId;

	bool isCasting;
	AActor* InteractionTarget;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Animation")
	bool IsMoving = false;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Levels")
	void TravelToLevel(FName LevelToLoad);

	UFUNCTION(Category = "Utility")
	void OnFinalCall();

	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
	float GetDisplayHealth();

	UFUNCTION(BlueprintCallable, Category = "PlayerStats")
	float GetDisplayMana();

	void SetPlayerId(uint8 Id);

	UFUNCTION(BlueprintCallable)
	uint8 GetPlayerId();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetIsCasting(bool _isCasting);

	UFUNCTION(Server, Reliable, WithValidation)
	void SetInteractionTarget(AActor* Target);

	AActor* GetInteractionTarget();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool IsCasting();

	ASDNetPlayerController *GetServerController();

	UFUNCTION(BlueprintCallable, Category = "Player")
	ASDNetPlayerPawn *GetServerCharacter();

	UFUNCTION(Reliable, Client, WithValidation)
	void SetLerpTarget(FVector target);
	void SetLerpTarget_Implementation(FVector target);
	bool SetLerpTarget_Validate(FVector target);

	void SetServerController(ASDNetPlayerController *NetControllerS);
	void SetServerController_Implementation(ASDNetPlayerController *NetControllerS);
	bool SetServerController_Validate(ASDNetPlayerController *NetControllerS);

	ASDBaseSpell* GetSpellSlot(uint8 Slot);
};
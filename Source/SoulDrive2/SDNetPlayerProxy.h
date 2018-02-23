// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Class.h"
#include "SDNetPlayerPawn.h"
#include "SDNetPlayerController.h"
#include "SDBaseEquipment.h"
#include "SDConstants.h"
#include "SDNetPlayerProxy.generated.h"

UCLASS()
class SOULDRIVE2_API ASDNetPlayerProxy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASDNetPlayerProxy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* PlayerCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* MainCameraBoom;

	ASDNetPlayerPawn *ServerCharacter;
	ASDNetPlayerController *ServerController;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TSubclassOf<ASDNetPlayerPawn> NetCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TSubclassOf<ASDNetPlayerController> NetControllerClass;

	TArray<ASDBaseEquipment *> CarriedItems;

	ASDNetPlayerController *GetServerController();

	UFUNCTION(Reliable, Client, WithValidation)
	void SetServerController(ASDNetPlayerController *NetControllerS);
	void SetServerController_Implementation(ASDNetPlayerController *NetControllerS);
	bool SetServerController_Validate(ASDNetPlayerController *NetControllerS);
	void PickupItem(const ASDBaseEquipment &AddedItem);


};
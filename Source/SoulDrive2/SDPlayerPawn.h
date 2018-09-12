// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SDBasePawn.h"
#include "SDBaseSpell.h"
#include "SDPlayerPawn.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDPlayerPawn : public ASDBasePawn
{
	GENERATED_BODY()
	
private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* PlayerCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* MainCameraBoom;

	void BeginPlay() override;

public:

	ASDPlayerPawn();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetPlayerCameraComponent() const { return PlayerCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetMainCameraBoom() const { return MainCameraBoom; }
	
	UFUNCTION(Reliable, Server, WithValidation)
	void SetPlayerWalkingSpeed();
	void SetPlayerWalkingSpeed_Implementation();
	bool SetPlayerWalkingSpeed_Validate();

};

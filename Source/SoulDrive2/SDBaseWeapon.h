// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SDBaseEquipment.h"
#include "SDConstants.h"
#include "SDBaseWeapon.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDBaseWeapon : public ASDBaseEquipment
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	WeaponRequirements WeaponType;
	int DamageLow;
	int DamageHigh;
};

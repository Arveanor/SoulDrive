// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SDBasePawn.h"
#include "SDBaseEquipment.h"
#include "SDBaseWeapon.h"
#include "UnrealNetwork.h"
#include "SDNetPlayerPawn.generated.h"

UENUM(BlueprintType)
enum class EEquipSlot : uint8
{
	MainWeaponMainHand UMETA(DisplayName = "MainWeapon_0"),
	MainWeaponOffHand UMETA(DisplayName = "MainWeapon_1"),
	AltWeaponMainHand UMETA(DisplayName = "AltWeapon_0"),
	AltWeaponOffHand UMETA(DisplayName = "AltWeapon_1"),
	Shoulder UMETA(DisplayName = "Shoulder"),
};

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDNetPlayerPawn : public ASDBasePawn
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	ASDNetPlayerPawn();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Levels")
	void TravelToLevel(FName LevelToLoad);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool DropItem(ASDBaseEquipment *Equipment);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void EquipItem(ASDBaseEquipment *Item, uint8 Slot);

	UFUNCTION(Category = "Items")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(Category = "Debug")
	void OnHitDetected(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetMainWeapon(ASDBaseWeapon *Weapon, bool bMainHand);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetAltWeapon(ASDBaseWeapon *Weapon, bool bMainHand);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetIsCasting(bool isCasting);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UnequipItem(ASDBaseEquipment *TargetItem);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool IsCasting();

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerDestroy();

	UFUNCTION(Server, Reliable, WithValidation)
	void SetInteractionTarget(AActor* Target);

	AActor* GetInteractionTarget();

	void SwapWeapons();

	ASDBaseWeapon* GetMainWeapon();
	ASDBaseWeapon* GetAltWeapon();


	UPROPERTY()
	TArray<ASDBaseEquipment *> CarriedItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Animation")
	bool IsMoving = false;
private:

	UFUNCTION(Server, Unreliable, WithValidation)
	void SetPortalTarget(const FString& InURL, ASDPortal* Portal);

	bool IsSpellCasting;
	TArray<ASDBaseWeapon *> MainWeapons;
	TArray<ASDBaseWeapon *> AltWeapons;
	TArray<ASDBaseWeapon*>* CurrentWeaponSet;

	TArray<ASDBaseEquipment *> ConvertItemStruct(const TArray<FItemStruct> &StructList);

	AActor* InteractionTarget;
};

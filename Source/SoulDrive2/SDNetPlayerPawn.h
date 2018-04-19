// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SDBasePawn.h"
#include "SDBaseEquipment.h"
//#include "SDNetPlayerControllerProxy.h"
#include "SDNetPlayerPawn.generated.h"

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

	UFUNCTION(BlueprintCallable, Category = "Levels")
	void TravelToLevel(FName LevelToLoad);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool DropItem(ASDBaseEquipment *Equipment);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void EquipItem(AActor *Item);

	void SetProxyController(APlayerController *ClientController);

	UFUNCTION(Category = "Items")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(Category = "Debug")
	void OnHitDetected(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& SweepResult);

	TArray<ASDBaseEquipment *> *CarriedItems;
	
private:
	APlayerController* ProxyController;
};

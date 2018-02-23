// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SDStatSheet.h"
#include "SDBaseEquipment.generated.h"

UCLASS()
class SOULDRIVE2_API ASDBaseEquipment : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDBaseEquipment();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	FString MeshName;
	SDStatSheet ItemDetails;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Player, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComp;
};

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

	UFUNCTION(Category = "Items")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Items")
	void SetActiveInWorld(bool active);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	FString MeshName;
	SDStatSheet ItemDetails;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComp;
};

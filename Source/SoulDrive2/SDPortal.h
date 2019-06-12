// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SDClickable.h"
#include "SDPortal.generated.h"


UCLASS()
class SOULDRIVE2_API ASDPortal : public AActor, public ISDClickable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDPortal();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool ReceiveClick(ASDNetPlayerControllerProxy* Controller);

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(BlueprintReadWrite)
	class UBoxComponent* Box;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Unreliable, WithValidation)
	void SetExistingURL(const FString &InURL);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FName TargetLevel;	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FString ExistingURL; // If someone's already gone this way we want to travel to their instance rather than loading a new one
	
};

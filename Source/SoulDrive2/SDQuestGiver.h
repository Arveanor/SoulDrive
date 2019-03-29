// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SDClickable.h"
#include "SUserWidget.h"
#include "SDQuestGiver.generated.h"

UCLASS()
class SOULDRIVE2_API ASDQuestGiver : public AActor, public ISDClickable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDQuestGiver();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, Category = "Widgets")
	USDQuestContainerWidget* QuestDialogue;

	UPROPERTY(BlueprintReadWrite, Category = "Widgets")
	USDQuestContainerWidget* CompleteDialogue;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class USDQuestContainerWidget> wQuestDialogue;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")	
	TSubclassOf<class USDQuestContainerWidget> wCompleteDialogue;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool ReceiveClick(ASDNetPlayerControllerProxy* Controller);

	UFUNCTION(BlueprintCallable)
	void GiveQuest(ASDNetPlayerControllerProxy* Controller);

	UFUNCTION(BlueprintCallable)
	void AdvanceQuest(ASDNetPlayerControllerProxy* Controller, USDBaseQuest* Quest);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Player, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComp;

	UPROPERTY()
	TArray<USDBaseQuest *> QuestsAvailable;

	bool ContainsAllQuests(TArray<USDBaseQuest *> SearchIn, TArray<USDBaseQuest *> SearchFor);
	int ContainsQuestName(TArray<USDBaseQuest *> QuestList, USDBaseQuest* Quest);
	
};

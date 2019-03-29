// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SDBaseQuest.generated.h"

UENUM(BlueprintType)
enum class EQuestStatus : uint8
{
	Active UMETA(DisplayName = "Active"),
	Failed UMETA(DisplayName = "Failed"),
	Complete UMETA(DisplayName = "Complete")
};

USTRUCT(BlueprintType)
struct FQuestStruct {
	GENERATED_BODY()

	UPROPERTY()
	EQuestStatus Status;

	UPROPERTY()
	FName RequiredItemName;

	UPROPERTY()
	FName QuestName;

	FQuestStruct() {}

	inline bool operator==(const FQuestStruct& Other) const
	{
		return Other.QuestName == QuestName;
	}
};

UCLASS()
class SOULDRIVE2_API USDBaseQuest : public UObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	USDBaseQuest();

	UPROPERTY(BlueprintReadWrite)
	FName QuestName;

public:	
	UFUNCTION(BlueprintCallable)
	virtual bool TurnInItem(ASDBaseEquipment *Item);

	FName GetRequiredItemName();
	void SetRequiredItemName(FName ItemName);

	EQuestStatus QuestStatus;

private:

	FName RequiredItemName;
	FName ReturnItemTo;

};

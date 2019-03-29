// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/Widget.h"
#include "SDBaseQuest.h"
#include "SDQuestContainerWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API USDQuestContainerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
	USDBaseQuest* ContainedQuest;
		
};

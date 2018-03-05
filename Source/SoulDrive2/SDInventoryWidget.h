// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "SDBaseEquipment.h"
#include "SDInventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API USDInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent, Category = "Widgets")
	void AddButtonToPanel(ASDBaseEquipment *ItemRef);
	
	
};

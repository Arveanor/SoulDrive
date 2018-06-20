// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/InstancedStaticMeshComponent.h"
#include "SDInstancedStaticMeshComponent.generated.h"

/**
 * 
 */
UCLASS(meta=(BlueprintSpawnableComponent))
class SOULDRIVE2_API USDInstancedStaticMeshComponent : public UInstancedStaticMeshComponent
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Util")
	int InstanceIndex;
	
	
};

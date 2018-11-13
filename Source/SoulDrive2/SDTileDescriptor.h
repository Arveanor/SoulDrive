// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SDTileDescriptor.generated.h"

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API ASDTileDescriptor : public AActor
{
	GENERATED_BODY()

public:
	ASDTileDescriptor();

	UPROPERTY(BlueprintReadWrite)
	FName name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int rotation;
// 
// 	UPROPERTY(BlueprintReadWrite)
// 	ProceduralTileEdges TopEdge;
// 
// 	UPROPERTY(BlueprintReadWrite)
// 	ProceduralTileEdges LeftEdge;
// 
// 	UPROPERTY(BlueprintReadWrite)
// 		ProceduralTileEdges RightEdge;
// 
// 	UPROPERTY(BlueprintReadWrite)
// 		ProceduralTileEdges BottomEdge;
};

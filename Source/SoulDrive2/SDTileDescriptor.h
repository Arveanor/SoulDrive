// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SDTileDescriptor.generated.h"

UENUM(BlueprintType)
enum class ProceduralTileEdges : uint8 {
	Open,
	Hall_Door,
	Hall_Open,
	Hall_Side,
	Wall_Main,
};

/**
 * 
 */
UCLASS()
class SOULDRIVE2_API USDTileDescriptor : public UObject
{
	GENERATED_BODY()

public:
	USDTileDescriptor();

	FName name;
	int rotation;
	ProceduralTileEdges TopEdge;
	ProceduralTileEdges LeftEdge;
	ProceduralTileEdges RightEdge;
	ProceduralTileEdges BottomEdge;

	ProceduralTileEdges GetFacingEdge(int direction);
	void CopyDescriptorData(USDTileDescriptor* CopyFrom);
	bool isValidForNeighbors(TArray<ProceduralTileEdges> neighbors);
};

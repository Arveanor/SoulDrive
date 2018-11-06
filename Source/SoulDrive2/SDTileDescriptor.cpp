// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDTileDescriptor.h"

USDTileDescriptor::USDTileDescriptor()
{
}

ProceduralTileEdges USDTileDescriptor::GetFacingEdge(int direction)
{
	
	direction -= rotation;
	direction = (direction + 360) % 360;
	switch (direction)
	{
	case 0:
		return TopEdge;
		break;
	case 90:
		return RightEdge;
		break;
	case 180:
		return BottomEdge;
		break;
	case 270:
		return LeftEdge;
		break;
	default:
		return TopEdge;
	}
}

void USDTileDescriptor::CopyDescriptorData(USDTileDescriptor* CopyFrom)
{
	TopEdge = CopyFrom->TopEdge;
	RightEdge = CopyFrom->RightEdge;
	BottomEdge = CopyFrom->BottomEdge;
	LeftEdge = CopyFrom->LeftEdge;
	rotation = CopyFrom->rotation;
}

bool USDTileDescriptor::isValidForNeighbors(TArray<ProceduralTileEdges> neighbors)
{
	return true;
}

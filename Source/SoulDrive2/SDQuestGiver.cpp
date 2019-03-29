// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDQuestGiver.h"


// Sets default values
ASDQuestGiver::ASDQuestGiver()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> RootMesh(TEXT("'/Engine/BasicShapes/Plane.Plane'"));
	MeshComp->SetStaticMesh(RootMesh.Object);
	RootComponent = MeshComp;

}

// Called when the game starts or when spawned
void ASDQuestGiver::BeginPlay()
{
	Super::BeginPlay();
	USDBaseQuest* FirstQuest = NewObject<USDBaseQuest>();
	FirstQuest->QuestName = FName("TestQuest");
	QuestsAvailable.Add(FirstQuest);
}

// Called every frame
void ASDQuestGiver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASDQuestGiver::GiveQuest(ASDNetPlayerControllerProxy* Controller)
{
	if (QuestsAvailable.Num() > 0)
	{
		Controller->AddQuest(QuestsAvailable[0]);
	}
}

void ASDQuestGiver::AdvanceQuest(ASDNetPlayerControllerProxy* Controller, USDBaseQuest* Quest)
{
}

bool ASDQuestGiver::ContainsAllQuests(TArray<USDBaseQuest *> SearchIn, TArray<USDBaseQuest*> SearchFor)
{
	for (USDBaseQuest *Q : SearchFor)
	{
		if (ContainsQuestName(SearchIn, Q) < 0)
		{
			return false;
		}
	}
	return true;
}

int ASDQuestGiver::ContainsQuestName(TArray<USDBaseQuest *> QuestList, USDBaseQuest* Quest)
{
	for (USDBaseQuest* Q : QuestList)
	{
		if (Quest->QuestName == Q->QuestName)
		{
			int result = QuestList.Find(Q);
			return result;
		}
	}
	return -1;
}

bool ASDQuestGiver::ReceiveClick_Implementation(ASDNetPlayerControllerProxy* Controller)
{
 	if (QuestsAvailable.Num() > 0 && ContainsAllQuests(Controller->GetAllQuests(), QuestsAvailable))
	{
		if (wCompleteDialogue)
		{
			if (CompleteDialogue == nullptr)
			{
				CompleteDialogue = CreateWidget<USDQuestContainerWidget>(Controller, wCompleteDialogue);
			}
			if (!CompleteDialogue->IsInViewport())
			{
				if ((QuestDialogue != nullptr && !QuestDialogue->IsInViewport()) || QuestDialogue == nullptr)
				{
					CompleteDialogue->AddToViewport();
					CompleteDialogue->ContainedQuest = QuestsAvailable[0];
					return true;
				}
			}
		}
	}
	else
	{
		if (wQuestDialogue)
		{
			if (QuestDialogue == nullptr)
			{
				QuestDialogue = CreateWidget<USDQuestContainerWidget>(Controller, wQuestDialogue);
			}
			if (!QuestDialogue->IsInViewport())
			{
				QuestDialogue->AddToViewport();
				if (QuestsAvailable.Num() > 0)
				{
					QuestDialogue->ContainedQuest = QuestsAvailable[0];
				}
			}
		}
		return true;
	}
	return false;
}


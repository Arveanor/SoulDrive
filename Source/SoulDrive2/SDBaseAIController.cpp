// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDBaseAICharacter.h"
#include "SDBaseAIController.h"
#include "SDBasePawn.h"


ASDBaseAIController::ASDBaseAIController(const FObjectInitializer &FOI)
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Component"));
}

void ASDBaseAIController::Possess(class APawn *InPawn)
{
	Super::Possess(InPawn);
	ASDBaseAICharacter* Bot = Cast<ASDBaseAICharacter>(InPawn);
	if (Bot != nullptr && Bot->BotBehavior != nullptr)
	{
		BlackboardComp->InitializeBlackboard(*(Bot->BotBehavior->BlackboardAsset));
		EnemyKeyID = BlackboardComp->GetKeyID("Enemy");
		EnemyLocationID = BlackboardComp->GetKeyID("Destination");

		BehaviorComp->StartTree(*(Bot->BotBehavior));
	}
}

void ASDBaseAIController::setEnemy(APawn * InPawn)
{
	if (BlackboardComp)
	{
 		BlackboardComp->SetValueAsObject("TargetEnemy", InPawn);
 		BlackboardComp->SetValueAsVector("Destination", InPawn->GetActorLocation());
	}
}

void ASDBaseAIController::searchForEnemy()
{
	APawn *MyBot = GetPawn();
	UE_LOG(LogTemp, Warning, TEXT("Searching For Enemy!"));
	if (MyBot == nullptr)
	{
		return;
	}

	const FVector MyLoc = MyBot->GetActorLocation();
	float bestDistSq = MAX_FLT;
	ASDBasePawn *BestPawn = nullptr;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		ASDBasePawn* TestPawn = Cast<ASDBasePawn>(*It);
		if (TestPawn)
		{
			const float DistSq = FVector::Dist(TestPawn->GetActorLocation(), MyLoc);
			if (DistSq < bestDistSq)
			{
				bestDistSq = DistSq;
				BestPawn = TestPawn;
			}
		}
	}
	if (BestPawn)
	{
		setEnemy(BestPawn);
	}
}
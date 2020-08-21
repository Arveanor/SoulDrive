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

// void ASDBaseAIController::Possess(class APawn *InPawn)
// {
// 	Super::Possess(InPawn);
// 	ASDBaseAICharacter* Bot = Cast<ASDBaseAICharacter>(InPawn);
// 	if (Bot != nullptr && Bot->BotBehavior != nullptr)
// 	{
// 		BlackboardComp->InitializeBlackboard(*(Bot->BotBehavior->BlackboardAsset));
// 		EnemyKeyID = BlackboardComp->GetKeyID("TargetEnemy");
// 		EnemyLocationID = BlackboardComp->GetKeyID("Destination");
// 
// 		BehaviorComp->StartTree(*(Bot->BotBehavior));
// 	}
// }

void ASDBaseAIController::setEnemy(APawn * InPawn)
{
	if (BlackboardComp)
	{
		StopMovement();
		BlackboardComp->SetValueAsObject(FName("TargetEnemy"), InPawn);
 		BlackboardComp->SetValueAsVector("Destination", InPawn->GetActorLocation());
	}
}

void ASDBaseAIController::searchForEnemy()
{
	APawn *MyBot = GetPawn();
	ASDBasePawn *SDBot = dynamic_cast<ASDBasePawn *>(MyBot);
	if (SDBot == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot cannot be cast to sd base pawn"));
		return;
	}
	const FVector MyLoc = SDBot->GetActorLocation();
	float bestDistSq = MAX_FLT;
	ASDBasePawn *BestPawn = nullptr;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		ASDBasePawn* TestPawn = Cast<ASDBasePawn>(*It);
		if (TestPawn)
		{
			if (TestPawn->GetTeamId() != SDBot->GetTeamId())
			{
				const float DistSq = FVector::Dist(TestPawn->GetActorLocation(), MyLoc);
				if (DistSq < bestDistSq)
				{
					bestDistSq = DistSq;
					BestPawn = TestPawn;
				}
			}
		}
	}
	if (BestPawn)
	{
		setEnemy(BestPawn);
	}
}
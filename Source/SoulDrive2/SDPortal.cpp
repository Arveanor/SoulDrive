// Fill out your copyright notice in the Description page of Project Settings.

#include "SoulDrive2.h"
#include "SDPortal.h"
#include "Runtime/Sockets/Public/SocketSubsystem.h"
#include "Runtime/Sockets/Public/IPAddress.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Address.h"

// Sets default values
ASDPortal::ASDPortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));

	Box->OnComponentBeginOverlap.AddDynamic(this, &ASDPortal::OnBoxBeginOverlap);
	Box->SetBoxExtent(FVector(100.f, 100.f, 100.f), true);
}

bool ASDPortal::ReceiveClick_Implementation(ASDNetPlayerControllerProxy* Controller)
{
	Controller->SetInteractionTarget(this);
	return true;
}

void ASDPortal::OnBoxBeginOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ASDNetPlayerPawn* OverlapPawn = dynamic_cast<ASDNetPlayerPawn *>(OtherActor);
	if (OverlapPawn != nullptr)
	{
		if (OverlapPawn->GetInteractionTarget() == this)
		{
			OverlapPawn->TravelToLevel(TargetLevel);
			// Here we'll want to send a message over, but let's just send a quick message to 127.0.0.1 for now
			FSocket* Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
			FIPv4Address ip(127, 0, 0, 1);

			TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
			addr->SetIp(ip.Value);
			addr->SetPort(7776);
			bool connected = Socket->Connect(*addr);

			FString TestMsg = TEXT("Hello, internet!");
			TCHAR* serializedChar = TestMsg.GetCharArray().GetData();
			int32 sent = 0;
			bool successful = Socket->Send((uint8*)TCHAR_TO_UTF8(serializedChar), FCString::Strlen(serializedChar), sent);
		}
	}
}

// Called when the game starts or when spawned
void ASDPortal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASDPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASDPortal::SetExistingURL_Implementation(const FString &InURL)
{
	ExistingURL = InURL;
}

bool ASDPortal::SetExistingURL_Validate(const FString &InURL)
{
	return true;
}


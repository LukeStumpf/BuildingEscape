// Copyright Luke Stumpf 2020

#include "OpenDoor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle += InitialYaw;

	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the open door component on it, but no pressureplate set"), *GetOwner()->GetName());
	}

	ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PressurePlate && PressurePlate->IsOverlappingActor(ActorThatOpens))
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}	
	else 
	{
		// if the door has been open longer than x seconds
		if ((GetWorld()->GetTimeSeconds() - DoorLastOpened) > DoorCloseDelay) {
			CloseDoor(DeltaTime);
		} 
	}
	
}

void UOpenDoor::OpenDoor(float DeltaTime) 
{
	// UE_LOG(LogTemp, Warning, TEXT("Current Rotation: %f"), CurrentYaw);
	float OpenYaw = FMath::FInterpTo(CurrentYaw, OpenAngle, DeltaTime * OpenDoorSpeed, 2);
	GetOwner()->SetActorRotation(FRotator(0.f, OpenYaw, 0.f));
	CurrentYaw = OpenYaw;
}

void UOpenDoor::CloseDoor(float DeltaTime) 
{
	// UE_LOG(LogTemp, Warning, TEXT("Current Rotation: %f"), CurrentYaw);
	float CloseYaw = FMath::FInterpTo(CurrentYaw, InitialYaw, DeltaTime * CloseDoorSpeed, 2);
	GetOwner()->SetActorRotation(FRotator(0.f, CloseYaw, 0.f));
	CurrentYaw = CloseYaw;
}


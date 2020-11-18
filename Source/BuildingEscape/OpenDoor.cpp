// Copyright Luke Stumpf 2020

#include "OpenDoor.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

#define OUT

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

	AActor * Owner = GetOwner();
	InitialYaw = Owner->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle += InitialYaw;

	FindPressurePlate();
	FindAudioComponent();
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Missing Audio component!"), *GetOwner()->GetName());
	}
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TotalMassOfActors() > MassToOpenDoors)
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}	
	else 
	{
		// if the door has been open longer than x seconds
		if ((GetWorld()->GetTimeSeconds() - DoorLastOpened) > DoorCloseDelay) 
		{
			CloseDoor(DeltaTime);
		} 
	}
}

void UOpenDoor::OpenDoor(float DeltaTime) 
{
	float OpenYaw = FMath::FInterpTo(CurrentYaw, OpenAngle, DeltaTime * OpenDoorSpeed, 2);
	GetOwner()->SetActorRotation(FRotator(0.f, OpenYaw, 0.f));
	CurrentYaw = OpenYaw;

	CloseDoorSoundPlayed = false;
	if (!AudioComponent) {return;}
	if (!OpenDoorSoundPlayed)
	{
		AudioComponent->Play();
		OpenDoorSoundPlayed = true;
	}
}

void UOpenDoor::CloseDoor(float DeltaTime) 
{
	float CloseYaw = FMath::FInterpTo(CurrentYaw, InitialYaw, DeltaTime * CloseDoorSpeed, 2);
	GetOwner()->SetActorRotation(FRotator(0.f, CloseYaw, 0.f));
	CurrentYaw = CloseYaw;

	OpenDoorSoundPlayed = false;
	if (!AudioComponent) {return;}
	if (!CloseDoorSoundPlayed)
	{
		AudioComponent->Play();
		CloseDoorSoundPlayed = true;
	}
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	// Find All Overlapping Actors.
	TArray<AActor*> OverlappingActors;
	if(!PressurePlate) {return TotalMass;}
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	// Add Up Their Masses.

	for (AActor* Actor : OverlappingActors) 
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("%s is on the pressureplate!"), *Actor->GetName());
	}

	return TotalMass;
}

void UOpenDoor::FindPressurePlate()
{
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the open door component on it, but no pressureplate set"), *GetOwner()->GetName());
		return;
	}
}
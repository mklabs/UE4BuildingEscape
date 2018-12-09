// Copyright Mickael Daniel 2018

#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Components/PrimitiveComponent.h"

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

	// Setup Objects
	OwningActor = GetOwner();

	// Setup Events
	if (!PressurePlate) {
		UE_LOG(LogTemp, Error, TEXT("No Component Pressure Plate attached for %s"), *OwningActor->GetName());
	} else {
		PressurePlate->OnActorBeginOverlap.AddDynamic(this, &UOpenDoor::OnOpenDoor);
		PressurePlate->OnActorEndOverlap.AddDynamic(this, &UOpenDoor::OnCloseDoor);
	}
}

void UOpenDoor::CloseDoor()
{
	UE_LOG(LogTemp, Warning, TEXT("Closing Door: %s"), *GetOwner()->GetName());

	// Set to initial rotation
	OnCloseRequest.Broadcast();
}

void UOpenDoor::OpenDoor()
{
	UE_LOG(LogTemp, Warning, TEXT("Opening Door: %s"), *GetOwner()->GetName());

	// Set new rotation
	OnOpenRequest.Broadcast();
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UOpenDoor::OnOpenDoor(AActor* ThisActor, AActor* OtherActor)
{
	UE_LOG(LogTemp, Warning, TEXT("OnOpen Door: %s"), *GetOwner()->GetName());

	if (GetTotalMassOfActorsOnPlate() >= PressurePlateMassToOpen) {
		OpenDoor();
	}
}

void UOpenDoor::OnCloseDoor(AActor * ThisActor, AActor * OtherActor)
{
	if (!PressurePlate) {
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("OnClose Door: %s"), *GetOwner()->GetName());

	// Get all overlapping actors
	TSet<AActor*> OverlappingActors;
	PressurePlate->GetOverlappingActors(OverlappingActors, AStaticMeshActor::StaticClass());

	// If not enough mass on overlapping actors, close the door
	if (GetTotalMassOfActorsOnPlate() < PressurePlateMassToOpen) {
		CloseDoor();
	}
}

float UOpenDoor::GetTotalMassOfActorsOnPlate()
{
	float TotalMass = 0.0f;

	if (!PressurePlate) {
		return TotalMass;
	}

	// Find all overlapping actors
	TSet<AActor*> OverlappingActors;
	PressurePlate->GetOverlappingActors(OverlappingActors);

	// Iterate through them adding their masses
	for (const auto* Actor : OverlappingActors) {
		UE_LOG(LogTemp, Warning, TEXT("Overlapping Actor: %s"), *Actor->GetName());
		UPrimitiveComponent* PrimitiveComponent = Actor->FindComponentByClass<UPrimitiveComponent>();
		TotalMass = TotalMass + PrimitiveComponent->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("New Mass: %f"), TotalMass);
	}

	return TotalMass;
}

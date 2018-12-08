// Copyright Mickael Daniel 2018

#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Engine/Public/TimerManager.h"

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
	ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();
	OwningActor = GetOwner();
	InitialRotation = GetOwner()->GetActorRotation();

	// Setup Events
	PressurePlate->OnActorBeginOverlap.AddDynamic(this, &UOpenDoor::OnOpenDoor);
	PressurePlate->OnActorEndOverlap.AddDynamic(this, &UOpenDoor::OnCloseDoor);
}

void UOpenDoor::CloseDoor()
{
	UE_LOG(LogTemp, Warning, TEXT("Closing Door: %s"), *GetOwner()->GetName());

	// Set to initial rotation
	OwningActor->SetActorRotation(InitialRotation, ETeleportType::None);
}

void UOpenDoor::OpenDoor()
{
	UE_LOG(LogTemp, Warning, TEXT("Opening Door: %s"), *GetOwner()->GetName());

	// Set new rotation
	OwningActor->SetActorRotation(FRotator(InitialRotation.Pitch, InitialRotation.Yaw + OpenAngle, InitialRotation.Roll), ETeleportType::None);
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UOpenDoor::OnOpenDoor(AActor* ThisActor, AActor* OtherActor)
{
	UE_LOG(LogTemp, Warning, TEXT("OnOpen Door: %s"), *GetOwner()->GetName());
	OpenDoor();
}

void UOpenDoor::OnCloseDoor(AActor * ThisActor, AActor * OtherActor)
{
	UE_LOG(LogTemp, Warning, TEXT("OnClose Door: %s"), *GetOwner()->GetName());
	FTimerHandle UnusedHandle;
	GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &UOpenDoor::CloseDoor, 1.0f, false, CloseDelay);
}

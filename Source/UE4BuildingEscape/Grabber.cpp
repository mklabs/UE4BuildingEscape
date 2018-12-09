// Copyright Mickael Daniel 2018

#include "Grabber.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) {
		return;
	}

	// If the physics handle is attached
	if (PhysicsHandle->GetGrabbedComponent()) {
		// Move the object that we're holding
		FPlayerViewPoint PlayerViewPoint = GetPlayerViewPoint();
		PhysicsHandle->SetTargetLocation(GetLineTraceEnd(50));
	}

}

void UGrabber::FindPhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle) {
		UE_LOG(LogTemp, Error, TEXT("%s is missing Physics Handle Component"), *GetOwner()->GetName());
	}
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (!InputComponent) {
		UE_LOG(LogTemp, Error, TEXT("%s is missing Input Component"), *GetOwner()->GetName());
	} else {
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Ungrab);
	}
}

void UGrabber::Grab()
{
	if (!PhysicsHandle) {
		return;
	}

	if (PhysicsHandle->GetGrabbedComponent()) {
		// If we have grabbed a component previously, release it
		PhysicsHandle->ReleaseComponent();
	} else {
		// Try and reach any actors with physics body collision channel set
		FHitResult HitResult = GetFirstPhysicsBodyInReach();

		AActor* HitActor = HitResult.GetActor();
		if (!HitActor) {
			return;
		}

		// If we hit something then attach a physics handle
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			HitResult.GetComponent(),
			NAME_None,
			HitActor->GetActorLocation(),
			// allow rotation
			HitActor->GetActorRotation()
		);
	}
}

void UGrabber::Ungrab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab key released"));
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	FPlayerViewPoint PlayerViewPoint = GetPlayerViewPoint();
	FVector LineTraceEnd = GetLineTraceEnd(0);

	// Line Trace (Ray-cast) out to reach distance
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByObjectType(
		HitResult,
		PlayerViewPoint.Location,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		FCollisionQueryParams(FName(TEXT("")), false, GetOwner())
	);

	return HitResult;
}

FPlayerViewPoint UGrabber::GetPlayerViewPoint()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotation);

	FPlayerViewPoint ViewPoint = FPlayerViewPoint();
	ViewPoint.Location = PlayerViewPointLocation;
	ViewPoint.Rotation = PlayerViewPointRotation;
	return ViewPoint;
}

FVector UGrabber::GetLineTraceEnd(int32 ReachMultiplierOffset)
{
	FPlayerViewPoint PlayerViewPoint = GetPlayerViewPoint();
	return PlayerViewPoint.Location + (PlayerViewPoint.Rotation.Vector() * (Reach + ReachMultiplierOffset));
}
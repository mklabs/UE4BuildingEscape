// Copyright Mickael Daniel 2018

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/InputComponent.h"
#include "Grabber.generated.h"

struct FPlayerViewPoint
{
	FVector Location;
	FRotator Rotation;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE4BUILDINGESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// Used to calculate the LineTraceEnd from the player
	UPROPERTY(EditAnywhere)
	float Reach = 100.0f;

	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	UInputComponent* InputComponent = nullptr;

	// Ray-cast and grab what's in reach
	void Grab();
	void Ungrab();

	// Find attached physics handle
	void FindPhysicsHandleComponent();

	// Setup (assumed) input component
	void SetupInputComponent();

	// Return Hit for first physics body in reach
	FHitResult GetFirstPhysicsBodyInReach();

	// Returns Location / Rotation of the Player
	FPlayerViewPoint GetPlayerViewPoint();

	// Returns the Vector to for ending Line Tracing or Ray-Cast
	FVector GetLineTraceEnd(int32 ReachMultiplierOffset);
};

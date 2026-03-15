#include "InteractionComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bIsHoldingObject = false;
}


// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Find the Physics Handle attached to the owning character
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("InteractionComponent requires a PhysicsHandleComponent on the owner actor. Please add one in Blueprints."));
	}

	// Find the Camera Component
	PlayerCamera = GetOwner()->FindComponentByClass<UCameraComponent>();
	if (!PlayerCamera)
	{
		UE_LOG(LogTemp, Error, TEXT("InteractionComponent requires a CameraComponent on the owner actor."));
	}
}


// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update the hold location if we are carrying an object
	if (bIsHoldingObject && PhysicsHandle && PlayerCamera)
	{
		FVector HoldLocation = PlayerCamera->GetComponentLocation() + (PlayerCamera->GetForwardVector() * HoldDistance);
		PhysicsHandle->SetTargetLocation(HoldLocation);
	}
}

void UInteractionComponent::GetTraceData(FVector& OutTraceStart, FVector& OutTraceEnd) const
{
	if (PlayerCamera)
	{
		OutTraceStart = PlayerCamera->GetComponentLocation();
		OutTraceEnd = OutTraceStart + (PlayerCamera->GetForwardVector() * ReachDistance);
	}
	else
	{
		OutTraceStart = FVector::ZeroVector;
		OutTraceEnd = FVector::ZeroVector;
	}
}

void UInteractionComponent::Interact()
{
	if (!PhysicsHandle || !PlayerCamera) return;

	// If already holding something, drop/throw it instead of grabbing something new
	if (bIsHoldingObject)
	{
		Release(false); // Just drop it for the standard interact button, throwing can be bound separately
		return;
	}

	FVector TraceStart, TraceEnd;
	GetTraceData(TraceStart, TraceEnd);

	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("InteractTrace")), false, GetOwner());

	// Perform Line Trace (Raycast) looking for Physics Objects (PhysicsBody collision channel)
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_PhysicsBody,
		TraceParams
	);

	if (bHit && HitResult.GetComponent())
	{
		UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();

		// Check if it's simulating physics and can be moved
		if (ComponentToGrab->IsSimulatingPhysics())
		{
			// Wake up the rigid body if it was asleep
			ComponentToGrab->WakeAllRigidBodies();

			// Grab the component
			PhysicsHandle->GrabComponentAtLocationWithRotation(
				ComponentToGrab,
				NAME_None, // Bone name (none for generic objects)
				ComponentToGrab->GetComponentLocation(),
				ComponentToGrab->GetComponentRotation()
			);

			bIsHoldingObject = true;
		}
	}
}

void UInteractionComponent::Release(bool bThrow)
{
	if (!PhysicsHandle || !bIsHoldingObject) return;

	UPrimitiveComponent* GrabbedComponent = PhysicsHandle->GetGrabbedComponent();

	// Release the object
	PhysicsHandle->ReleaseComponent();
	bIsHoldingObject = false;

	// Apply throw force if requested
	if (bThrow && GrabbedComponent && PlayerCamera)
	{
		FVector ThrowDirection = PlayerCamera->GetForwardVector();
		GrabbedComponent->AddImpulse(ThrowDirection * ThrowForce, NAME_None, true); // true = velocity change, ignore mass
	}
}

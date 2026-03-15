#include "InteractableDoor.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AInteractableDoor::AInteractableDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Setup Door Frame (Root)
	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	RootComponent = DoorFrame;

	// Setup Door Panel
	DoorPanel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorPanel"));
	DoorPanel->SetupAttachment(RootComponent);
	// Panel MUST simulate physics for the hinge to work
	DoorPanel->SetSimulatePhysics(true);

	// Setup Physics Constraint (Hinge)
	DoorHinge = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("DoorHinge"));
	DoorHinge->SetupAttachment(RootComponent);

	// Default Settings
	bIsLocked = false;
	MaxOpenAngle = 90.0f;
	SlamForce = 50000.0f;
}

// Called when the game starts or when spawned
void AInteractableDoor::BeginPlay()
{
	Super::BeginPlay();

	// Connect the constraint at runtime
	if (DoorHinge && DoorFrame && DoorPanel)
	{
		DoorHinge->SetConstrainedComponents(DoorFrame, NAME_None, DoorPanel, NAME_None);
		UpdateConstraintLimits();
	}
}

// Called every frame
void AInteractableDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractableDoor::UpdateConstraintLimits()
{
	if (!DoorHinge) return;

	if (bIsLocked)
	{
		// Lock all angular motion
		DoorHinge->SetAngularSwing1Limit(ACM_Locked, 0.0f);
		DoorHinge->SetAngularSwing2Limit(ACM_Locked, 0.0f);
		DoorHinge->SetAngularTwistLimit(ACM_Locked, 0.0f);
	}
	else
	{
		// Unlock swing motion (acting like a hinge).
		// Note: Depending on mesh orientation, this might need to be Twist or Swing2.
		// Assuming standard UE orientation where Z is up (Swing1 = yaw).
		DoorHinge->SetAngularSwing1Limit(ACM_Limited, MaxOpenAngle);
		DoorHinge->SetAngularSwing2Limit(ACM_Locked, 0.0f);
		DoorHinge->SetAngularTwistLimit(ACM_Locked, 0.0f);
	}
}

void AInteractableDoor::SetDoorLocked(bool bNewLockedState)
{
	bIsLocked = bNewLockedState;
	UpdateConstraintLimits();
}

void AInteractableDoor::PushDoor(FVector PushDirection, float ForceMagnitude)
{
	if (!bIsLocked && DoorPanel)
	{
		// Apply an impulse to the door panel to swing it
		DoorPanel->AddImpulseAtLocation(PushDirection * ForceMagnitude, DoorPanel->GetComponentLocation());
	}
}

void AInteractableDoor::OnInteract_Implementation(AActor* InteractingActor)
{
	// Standard interaction: if it's unlocked, give it a little push to swing it open
	if (!bIsLocked && InteractingActor)
	{
		// Calculate direction from the player to the door
		FVector Direction = DoorPanel->GetComponentLocation() - InteractingActor->GetActorLocation();
		Direction.Z = 0.0f; // Keep it on a flat plane
		Direction.Normalize();

		// Apply a moderate push (like pushing a door slowly)
		PushDoor(Direction, SlamForce * 0.2f);
	}
	else if (bIsLocked)
	{
		// Door is locked! Could trigger a rattle sound effect or widget here in Blueprints
		UE_LOG(LogTemp, Warning, TEXT("The door is locked!"));
	}
}

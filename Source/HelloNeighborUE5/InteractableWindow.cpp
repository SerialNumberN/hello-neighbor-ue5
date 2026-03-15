#include "InteractableWindow.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

// Sets default values
AInteractableWindow::AInteractableWindow()
{
	PrimaryActorTick.bCanEverTick = false; // Doesn't need to tick

	// Create the visual glass mesh
	WindowGlass = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WindowGlass"));
	RootComponent = WindowGlass;

	// Create the collision box for hit detection
	BreakCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BreakCollision"));
	BreakCollision->SetupAttachment(RootComponent);
	BreakCollision->SetBoxExtent(FVector(10.0f, 100.0f, 100.0f));
	BreakCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	// Enable Hit Events
	BreakCollision->SetNotifyRigidBodyCollision(true);

	// Default settings
	BreakThreshold = 1000.0f;
	bIsBroken = false;
}

// Called when the game starts or when spawned
void AInteractableWindow::BeginPlay()
{
	Super::BeginPlay();

	// Bind the OnHit event
	if (BreakCollision)
	{
		BreakCollision->OnComponentHit.AddDynamic(this, &AInteractableWindow::OnWindowHit);
	}
}

// Called every frame
void AInteractableWindow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractableWindow::OnWindowHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bIsBroken) return; // Already broken

	// Calculate impact magnitude
	float ImpactForce = NormalImpulse.Size();

	// Alternatively, if NormalImpulse isn't giving reliable results with fast-moving low-mass objects (like a thrown shoe),
	// calculate force manually based on the other object's velocity and mass.
	if (OtherComp && OtherComp->IsSimulatingPhysics())
	{
		float Mass = OtherComp->GetMass();
		float Velocity = OtherComp->GetComponentVelocity().Size();

		// F = ma (simplified to just momentum p = mv for impact checking)
		float EstimatedForce = Mass * Velocity;

		// Use whichever is higher
		if (EstimatedForce > ImpactForce)
		{
			ImpactForce = EstimatedForce;
		}
	}

	// Check if the force exceeds the threshold to break the glass
	if (ImpactForce > BreakThreshold)
	{
		bIsBroken = true;

		// Disable collision on the solid glass so players and objects can jump/fly through
		WindowGlass->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BreakCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Hide the solid glass mesh (Blueprints will typically spawn shattered pieces here)
		WindowGlass->SetVisibility(false);

		// Play sound
		if (ShatterSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ShatterSound, GetActorLocation());
		}

		// Trigger Blueprint event for visual/audio effects (glass shattering, spawning chunks)
		OnWindowShatter();
	}
}

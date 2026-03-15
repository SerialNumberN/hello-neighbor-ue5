#include "NeighborAICharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "NeighborAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ANeighborAICharacter::ANeighborAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize the state
	CurrentState = ENeighborState::Patrolling;

	// Footsteps
	FootstepInterval = 0.5f; // Play a footstep every half second while moving
	LastFootstepTime = 0.0f;

	// Catch Mechanic Sphere
	CatchRadius = CreateDefaultSubobject<USphereComponent>(TEXT("CatchRadius"));
	CatchRadius->SetupAttachment(RootComponent);
	CatchRadius->InitSphereRadius(100.0f); // Default catch radius
	CatchRadius->SetCollisionProfileName(TEXT("Trigger"));
}

// Called when the game starts or when spawned
void ANeighborAICharacter::BeginPlay()
{
	Super::BeginPlay();

	// Bind the overlap event to the CatchRadius sphere
	if (CatchRadius)
	{
		CatchRadius->OnComponentBeginOverlap.AddDynamic(this, &ANeighborAICharacter::OnOverlapBegin);
	}
}

// Called every frame
void ANeighborAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Fallback footstep handling (ideally moved to Animation Notifies when you have animations)
	if (FootstepSound && GetVelocity().SizeSquared() > 100.0f)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime - LastFootstepTime >= FootstepInterval)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FootstepSound, GetActorLocation());
			LastFootstepTime = CurrentTime;
		}
	}
}

void ANeighborAICharacter::SetNeighborState(ENeighborState NewState)
{
	if (CurrentState != NewState)
	{
		// Handle State Transition Audio
		if (NewState == ENeighborState::Chasing && AlertSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, AlertSound, GetActorLocation());
		}
		else if (NewState == ENeighborState::Investigating && InvestigateSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, InvestigateSound, GetActorLocation());
		}
		else if (NewState == ENeighborState::Patrolling && PatrolSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PatrolSound, GetActorLocation());
		}
	}

	CurrentState = NewState;

	// Notify the controller that the state changed so it can update the Blackboard
	ANeighborAIController* AIController = Cast<ANeighborAIController>(GetController());
	if (AIController)
	{
		AIController->UpdateStateInBlackboard(CurrentState);
	}
}

void ANeighborAICharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Ensure we are chasing and hit the player
	if (CurrentState == ENeighborState::Chasing && OtherActor && OtherActor != this)
	{
		// Assuming the player character possesses the first player controller
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

		if (OtherActor == PlayerPawn)
		{
			// We caught the player!
			UE_LOG(LogTemp, Warning, TEXT("Neighbor caught the player!"));

			// Optional: Freeze the Neighbor immediately so they don't keep running past
			GetCharacterMovement()->StopMovementImmediately();

			// Trigger the blueprint implementable event
			OnPlayerCaught();
		}
	}
}

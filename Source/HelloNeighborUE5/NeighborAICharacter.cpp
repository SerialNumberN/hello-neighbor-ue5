#include "NeighborAICharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BehaviorTree.h"
#include "NeighborAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ANeighborAICharacter::ANeighborAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize the state
	CurrentState = ENeighborState::Patrolling;

	// Create and configure the AI Perception Component
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));

	// Configure Sight
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SightConfig->SightRadius = 1500.0f;
	SightConfig->LoseSightRadius = 2000.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->SetMaxAge(5.0f);
	SightConfig->AutoSuccessRangeFromLastSeenLocation = -1.0f;

	// Detect everything (enemies, neutrals, friendlies)
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// Configure Hearing
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	HearingConfig->HearingRange = 3000.0f;
	HearingConfig->SetMaxAge(3.0f);

	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// Add the senses to the perception component
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->ConfigureSense(*HearingConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseID());
}

// Called when the game starts or when spawned
void ANeighborAICharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AIPerceptionComponent)
	{
		// Bind the perception update function
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ANeighborAICharacter::OnTargetPerceptionUpdated);
	}

	// Bind the overlap event to the character's collision capsule for the Catch Mechanic
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ANeighborAICharacter::OnOverlapBegin);
	}
}

// Called every frame
void ANeighborAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANeighborAICharacter::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor || Actor == this) return;

	// Get the AI Controller controlling this character
	ANeighborAIController* AIController = Cast<ANeighborAIController>(GetController());
	if (AIController)
	{
		// Let the Controller handle the logic (updating Blackboard, changing state, etc.)
		AIController->HandlePerceptionUpdate(Actor, Stimulus);
	}
}

void ANeighborAICharacter::SetNeighborState(ENeighborState NewState)
{
	if (CurrentState != ENeighborState::Chasing && NewState == ENeighborState::Chasing)
	{
		// Just got alerted to the player
		if (AlertSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, AlertSound, GetActorLocation());
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

#include "NeighborAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"

ANeighborAIController::ANeighborAIController()
{
	// Create and configure the AI Perception Component
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));

	// Properly create the sense configs using CreateDefaultSubobject.
	// Using NewObject in the constructor is dangerous and deprecated in UE5 for subobjects.
	// This creates them correctly on the CDO.
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));

	if (SightConfig)
	{
		SightConfig->SightRadius = 1500.0f;
		SightConfig->LoseSightRadius = 2000.0f;
		SightConfig->PeripheralVisionAngleDegrees = 90.0f;
		SightConfig->SetMaxAge(5.0f);
		SightConfig->AutoSuccessRangeFromLastSeenLocation = -1.0f;

		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		AIPerceptionComponent->ConfigureSense(*SightConfig);
		AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseID());
	}

	if (HearingConfig)
	{
		HearingConfig->HearingRange = 3000.0f;
		HearingConfig->SetMaxAge(3.0f);

		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
		AIPerceptionComponent->ConfigureSense(*HearingConfig);
	}
}

void ANeighborAIController::BeginPlay()
{
	Super::BeginPlay();

	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ANeighborAIController::OnTargetPerceptionUpdated);
	}
}

void ANeighborAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Run the Behavior Tree using the standard AAIController function.
	// This ensures the base class Blackboard pointer is correctly set up.
	if (BehaviorTree && RunBehaviorTree(BehaviorTree))
	{
		// Get the now-initialized Blackboard from the base class
		BlackboardComponent = GetBlackboardComponent();

		if (BlackboardComponent)
		{
			// Initialize default state
			UpdateStateInBlackboard(ENeighborState::Patrolling);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Neighbor AI Controller possessed a pawn but no valid Behavior Tree is assigned!"));
	}
}

void ANeighborAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!BlackboardComponent) return;

	// Ignore perception entirely if the Neighbor is currently trapped
	if (BlackboardComponent->GetValueAsBool(IsTrappedKeyName))
	{
		return;
	}

	// Example logic: if the AI successfully sensed something
	if (Stimulus.WasSuccessfullySensed())
	{
		// Check if the sensed actor is the player character
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (Actor == PlayerPawn)
		{
			// The AI saw or heard the player! Update the Blackboard to trigger the Chasing logic in the Behavior Tree
			BlackboardComponent->SetValueAsObject(TargetActorKeyName, Actor);
			BlackboardComponent->SetValueAsVector(TargetLocationKeyName, Actor->GetActorLocation());

			// We check if the state is already chasing so we don't spam the alert sound
			// every time the perception ticks while chasing.
			ANeighborAICharacter* NeighborCharacter = Cast<ANeighborAICharacter>(GetPawn());
			if (NeighborCharacter && NeighborCharacter->CurrentState != ENeighborState::Chasing)
			{
				NeighborCharacter->SetNeighborState(ENeighborState::Chasing); // this triggers the alert sound and updates BB
			}
		}
		else
		{
			// The AI saw or heard something else (like a thrown box!). Update the Blackboard
			// so the Investigate Location Behavior Tree task can find it.
			BlackboardComponent->SetValueAsVector(TargetLocationKeyName, Stimulus.StimulusLocation);

			// Only investigate if we aren't already chasing the player
			ANeighborAICharacter* NeighborCharacter = Cast<ANeighborAICharacter>(GetPawn());
			if (NeighborCharacter && NeighborCharacter->CurrentState != ENeighborState::Chasing)
			{
				NeighborCharacter->SetNeighborState(ENeighborState::Investigating);
			}
		}
	}
	else
	{
		// Senses lost (e.g., player out of sight). Go into Searching mode.
		if (BlackboardComponent->GetValueAsObject(TargetActorKeyName) == Actor)
		{
			BlackboardComponent->SetValueAsVector(TargetLocationKeyName, Stimulus.StimulusLocation); // Remember last known location

			ANeighborAICharacter* NeighborCharacter = Cast<ANeighborAICharacter>(GetPawn());
			if (NeighborCharacter)
			{
				NeighborCharacter->SetNeighborState(ENeighborState::Investigating);
			}
		}
	}
}

void ANeighborAIController::UpdateStateInBlackboard(ENeighborState NewState)
{
	if (BlackboardComponent)
	{
		// Cast the Enum to an integer so it can be stored in the Blackboard
		BlackboardComponent->SetValueAsEnum(CurrentStateKeyName, (uint8)NewState);
	}
}

void ANeighborAIController::SetTrappedState(bool bIsTrapped)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsBool(IsTrappedKeyName, bIsTrapped);
	}
}

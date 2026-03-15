#include "NeighborAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

ANeighborAIController::ANeighborAIController()
{
	// The BehaviorTree and Blackboard components are created automatically
	// by the base AAIController when RunBehaviorTree is called.
}

void ANeighborAIController::BeginPlay()
{
	Super::BeginPlay();
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

void ANeighborAIController::HandlePerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	if (!BlackboardComponent) return;

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

			UpdateStateInBlackboard(ENeighborState::Chasing);
		}
	}
	else
	{
		// Senses lost (e.g., player out of sight). Go into Investigating or Searching mode.
		if (BlackboardComponent->GetValueAsObject(TargetActorKeyName) == Actor)
		{
			BlackboardComponent->ClearValue(TargetActorKeyName); // Forget the exact actor
			BlackboardComponent->SetValueAsVector(TargetLocationKeyName, Stimulus.ReceiverLocation); // Remember last known location

			UpdateStateInBlackboard(ENeighborState::Investigating);
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

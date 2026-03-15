#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "NeighborAICharacter.h" // Include enum and base class
#include "NeighborAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;
class UBehaviorTree;

UCLASS()
class HELLONEIGHBORUE5_API ANeighborAIController : public AAIController
{
	GENERATED_BODY()

public:
	ANeighborAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

public:
	// Handle perception updates from the character (Sight, Hearing)
	void HandlePerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);

	// Update the Blackboard with the current state (enum)
	void UpdateStateInBlackboard(ENeighborState NewState);

	// The Behavior Tree asset to run for this AI (Assign in Editor)
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;

private:
	// Pointer to cache the Blackboard component retrieved from the base class
	UBlackboardComponent* BlackboardComponent;

	// Keys for the Blackboard (these must match the names you create in the Editor Blackboard)
	const FName TargetLocationKeyName = "TargetLocation";
	const FName TargetActorKeyName = "TargetActor";
	const FName CurrentStateKeyName = "CurrentState";
};

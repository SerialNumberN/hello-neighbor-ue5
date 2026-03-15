#include "BTTask_InvestigateLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NeighborAIController.h"
#include "NeighborAICharacter.h"
#include "TimerManager.h"
#include "AIController.h"

UBTTask_InvestigateLocation::UBTTask_InvestigateLocation()
{
	NodeName = TEXT("Investigate Location");
	InvestigationWaitTime = 5.0f; // Default 5 seconds wait
	bNotifyTick = false; // We don't need tick, we use a timer
	bCreateNodeInstance = true; // Required since we are storing state (WaitTimerHandle) in the node itself
}

EBTNodeResult::Type UBTTask_InvestigateLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	// This task is asynchronous since the AI needs to wait.
	// We start a timer and finish the task when the timer is done.
	FTimerDelegate TimerDel;
	TimerDel.BindUObject(this, &UBTTask_InvestigateLocation::OnWaitFinished, &OwnerComp);

	GetWorld()->GetTimerManager().SetTimer(WaitTimerHandle, TimerDel, InvestigationWaitTime, false);

	// Return InProgress to tell the Behavior Tree to wait until we manually finish
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_InvestigateLocation::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Ensure the timer is cleared if the Behavior Tree is interrupted (e.g. AI spots player mid-investigation)
	GetWorld()->GetTimerManager().ClearTimer(WaitTimerHandle);

	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_InvestigateLocation::OnWaitFinished(UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp) return;

	ANeighborAIController* AIController = Cast<ANeighborAIController>(OwnerComp->GetAIOwner());
	if (AIController)
	{
		// If we reached here without the perception interrupting and changing the state to Chasing,
		// it means the investigation yielded nothing. We switch the state back to Patrolling.
		AIController->UpdateStateInBlackboard(ENeighborState::Patrolling);

		// Clear the target location from the blackboard since we finished investigating it
		OwnerComp->GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());

		// Finish the task successfully
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
	else
	{
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
	}
}
